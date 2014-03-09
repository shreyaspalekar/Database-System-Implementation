#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include <fstream>
#include <stdlib.h>
#include <string.h>

SortedFile::SortedFile () {
	inPipe = new Pipe(100);	
	outPipe = new Pipe(100);	
	bq = NULL;
	file = new File();
	readPageBuffer = new Page();
	tobeMerged = new Page();
	current = new Record();	
	m = R;
	isDirty=0;
}

void* SortedFile::instantiate_BigQ(void* arg){

	thread_arguments *args;
	args = (thread_arguments *) arg;

	//cout<<"check   "<<(args.s).runLength;
	//cout<<"t check "<<args->in<<"\n";

	args->b = new BigQ(*(args->in),*(args->out),*((args->s).myOrder),(args->s).runLength);

}

int SortedFile::Create (char *f_path, fType f_type, void *startup) {	// done
	file->Open(0,f_path);	

	fileName = (char *)malloc(sizeof(f_path)+1);
	strcpy(fileName,f_path);
	isDirty=0;
	
	// use startup to get runlength and ordermaker
	si = (SortInfo *) startup;
	pageIndex=1;
	endOfFile=1;
	return 1;
}

void SortedFile::Load (Schema &f_schema, char *loadpath) {		// requires BigQ instance done

	if(m!=W){
		m = W;
		isDirty=1;
		// create input, output pipe and BigQ instance
		//if(inPipe==NULL)inPipe = new Pipe(100);	// requires size ?
		//if(outPipe==NULL)outPipe = new Pipe(100);
		if(bq==NULL)bq = new BigQ(*inPipe,*outPipe,*(si->myOrder),si->runLength);
	}

	FILE* tableFile = fopen (loadpath,"r");
	Record temp;//need reference see below, make a record

	while(temp.SuckNextRecord(&f_schema,tableFile)!=0)
		inPipe->Insert(&temp);	// pipe blocks and record is consumed or is buffering required ?

	fclose(tableFile);	
}

int SortedFile::Open (char *f_path) {


	isDirty=0;
	char *fName = new char[20];
	sprintf(fName, "%s.meta", f_path);
//	FILE *f = fopen(fName,"r");

	fileName = (char *)malloc(sizeof(f_path)+1);
	strcpy(fileName,f_path);


	// to decide what to store in meta file
	// and parse and get sort order and run length
	// requires some kind of de serialization
	// initialize it

	ifstream ifs(fName,ios::binary);

	ifs.seekg(sizeof(fileName)-1);//,ifs.beg);

	ifs.read((char*)&si, sizeof(si)); 

	ifs.close();

	m = R;

	//MoveFirst();

	// set to read mode
	// bring first page into read buffer and initialize first record

	//fclose(f);

	file->Open(1, f_path);	// open the corresponding file
	pageIndex = 1;
	endOfFile = 0;
}

void SortedFile::MoveFirst () {			// requires MergeFromOuputPipe()

	isDirty=0;	
	pageIndex = 1;

	if(m==R){
		// In read mode, so direct movefirst is possible
		
		if(file->GetLength()!=0){
			file->GetPage(readPageBuffer,pageIndex); //TODO: check off_t type,  void GetPage (Page *putItHere, off_t whichPage)
	
			int result = readPageBuffer->GetFirst(current);
			//cout<<result<<endl;
		}

	}
	else{
		// change mode to read
		m = R;
		// Merge contents if any from BigQ
		MergeFromOutpipe();
		file->GetPage(readPageBuffer,pageIndex); //TODO: check off_t type,  void GetPage (Page *putItHere, off_t whichPage)
		readPageBuffer->GetFirst(current);
		// bring the first page into readPageBuffer
		// Set curr Record to first record
		// 
	}

}

int SortedFile::Close () {			// requires MergeFromOuputPipe()	done
	
	if(m==W)	
		MergeFromOutpipe();

	file->Close();
	isDirty=0;
	endOfFile = 1;
	// write updated state to meta file

	char fName[30];
	sprintf(fName,"%s.meta",fileName);

	ofstream out(fName);
    	out <<"sorted"<<endl;
    	out.close();


	ofstream ofs(fName,ios::binary|ios::app);

	ofs.write((char*) &si,sizeof(si));	

	ofs.close();
}

void SortedFile::Add (Record &rec) {	// requires BigQ instance		done

	//cout<<"check "<<inPipe<<"\n";
	

	inPipe->Insert(&rec);
	//inPipe->ShutDown();
	//cout<<m<<"\n";

	if(m!=W){
		isDirty=1;
		m = W;
		//cout<<m<<"\n";

		// create input, output pipe and BigQ instance
		/*if(inPipe==NULL){
			inPipe = new Pipe(100);	// requires size ?
			cout<<"Setting up input pipe\n";		
		}
		if(outPipe==NULL){
			outPipe = new Pipe(100);
			cout<<"Setting up output pipe\n";
		}*/

		inPipe= new Pipe(100);
		outPipe= new Pipe(100);

		if(bq==NULL){

			//cout<<"run length "<<si->runLength<<"\n";
			thread_args.in = inPipe;
			thread_args.out = outPipe;
			thread_args.s.myOrder = si->myOrder;
			thread_args.s.runLength =  si->runLength;
			thread_args.b = bq;

			

			pthread_create(&bigQ_t, NULL, &SortedFile::instantiate_BigQ , (void *)&thread_args);


			cout<<"Setting up BigQ"<<endl;
		}
	}


	//cout<<"record adr"<<&rec<<"\n";
	

	//cout << inPipe<<"\n";
	//inPipe= new Pipe(100);
	
		// pipe blocks and record is consumed or is buffering required ?

	//cout <<"Added\n";
}

int SortedFile::GetNext (Record &fetchme) {		// requires MergeFromOuputPipe()		done

	if(m!=R){
		isDirty=0;
		m = R;
		readPageBuffer->EmptyItOut();		// requires flush
		MergeFromOutpipe();		// 
		MoveFirst();	// always start from first record

	}

	if(endOfFile==1) return 0;

	fetchme.Copy(current);

	if(!readPageBuffer->GetFirst(current)) {

		if(pageIndex>=this->file->GetLength()-1){
				endOfFile = 1;
				return 0;	
		}
		else {
			pageIndex++;
			file->GetPage(readPageBuffer,pageIndex);
			readPageBuffer->GetFirst(current);

		}
	}

	return 1;
}

int SortedFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {		// requires binary search // requires MergeFromOuputPipe()
}

void SortedFile:: MergeFromOutpipe(){		// requires both read and write modes

	// close input pipe
	cout<<inPipe<<endl;

	inPipe->ShutDown();
	// get sorted records from output pipe
	ComparisonEngine *ce;

	// following four lines get the first record from those already present (not done)
	if(!tobeMerged){ tobeMerged = new Page(); }
	pagePtrForMerge = 0; 
	Record *rFromFile = new Record();
	GetNew(rFromFile);						// loads the first record from existing records

	Record *rtemp = new Record();		
	Page *ptowrite = new Page();			// new page that would be added
	File *newFile = new File();				// new file after merging
	newFile->Open(0,"mergedFile");				

	bool nomore = false;
        int result =GetNew(rFromFile);
	int pageIndex = 1;


	Schema nu("catalog","lineitem");


	if(result==0)
		nomore =true;

	while(isDirty!=0&&!nomore){

		

		if(outPipe->Remove(rtemp)==1){		// got the record from out pipe

			//rtemp->Print(&nu);

			while(ce->Compare(rFromFile,rtemp,si->myOrder)<0){ 		// merging this record with others

				if(ptowrite->Append(rFromFile)==0){		// merge already existing record
						// page full
						
						//int pageIndex = newFile->GetLength()==0? 0:newFile->GetLength()-1;

						//*
						// write this page to file
						newFile->AddPage(ptowrite,pageIndex++);
						//pageIndex++;
						// empty this out
						ptowrite->EmptyItOut();
						// append the current record ?
						ptowrite->Append(rtemp);		// does this consume the record ?
				}

				if(!GetNew(rFromFile)){ nomore = true; break; }	// bring next rFromFile record ?// check if records already present are exhausted

			}


			if(ptowrite->Append(rtemp)!=1){				// copy record from pipe
						// page full
						
						//int pageIndex = newFile->GetLength()==0? 0:newFile->GetLength()-1;


						//*
						// write this page to file
						newFile->AddPage(ptowrite,pageIndex++);
						// empty this out
						ptowrite->EmptyItOut();
						// append the current record ?
						ptowrite->Append(rtemp);		// does this consume the record ?
			}

		}
		else{
			// pipe is empty now, copy rest of records to new file
			do{
				if(ptowrite->Append(rFromFile)!=1){			

					//int pageIndex = newFile->GetLength()==0? 0:newFile->GetLength()-1;	// page full
					//*
					// write this page to file
					newFile->AddPage(ptowrite,pageIndex++);
					// empty this out
					ptowrite->EmptyItOut();
					// append the current record ?
					ptowrite->Append(rFromFile);		// does this consume the record ?
				}
			}while(GetNew(rFromFile)!=0);
			break;
		}
	}

	outPipe->Remove(rtemp);

	

	if(nomore==true){									// file is empty
		do{

			//rtemp->Print(&nu);	
		
			if(ptowrite->Append(rtemp)!=1){				// copy record from pipe
						//int pageIndex = newFile->GetLength()==0? 0:newFile->GetLength()-1;		// page full
						// write this page to file
						//cout<<"write at index "<<pageIndex<<endl;
						newFile->AddPage(ptowrite,pageIndex++);
						// empty this out
						ptowrite->EmptyItOut();
						// append the current record ?
						ptowrite->Append(rtemp);		// does this consume the record ?
			}
		}while(outPipe->Remove(rtemp)!=0);
	}

	newFile->AddPage(ptowrite,pageIndex);//newFile->GetLength()-1);

	newFile->Close();
	file->Close();

	// delete resources that are not required

	if(rename(fileName,"mergefile.tmp")!=0) {				// making merged file the new file
		cerr <<"rename file error!"<<endl;
		return;
	}
	
	remove("mergefile.tmp");

	if(rename("mergedFile",fileName)!=0) {				// making merged file the new file
		cerr <<"rename file error!"<<endl;
		return;
	}

	readPageBuffer->EmptyItOut();




	file->Open(1, this->fileName);

}


int SortedFile:: GetNew(Record *r1){

	while(!this->tobeMerged->GetFirst(r1)) {
		if(pagePtrForMerge >= file->GetLength()-1)
			return 0;
		else {
			file->GetPage(tobeMerged, pagePtrForMerge);
			pagePtrForMerge++;
		}
	}

	return 1;
}	


SortedFile::~SortedFile() {
	delete readPageBuffer;
	delete inPipe;
	delete outPipe;
}
