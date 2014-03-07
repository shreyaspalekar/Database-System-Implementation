#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "SortedFile.h"
#include "Defs.h"

SortedFile::SortedFile () {
	inPipe = null;
	outPipe = null;
	bq = null;
	readPageBuffer = new Page();
	//pageToBeMerged = new Page(); can be delayed ?
	m = R;
}

int SortedFile::Create (char *f_path, fType f_type, void *startup) {	// done
	file->Open(0,f_path);		
	// use startup to get runlength and ordermaker
	si = (SortInfo *) startup;
	pageIndex=1;
	endOfFile=1;
	return 1;
}

void SortedFile::Load (Schema &f_schema, char *loadpath) {		// requires BigQ instance done
	
	if(m!=Mode.W){
		m = Mode.W;
		// create input, output pipe and BigQ instance
		if(inPipe!=null)inPipe = new Pipe();	// requires size ?
		if(outPipe!=null)outPipe = new Pipe();
		if(bq!=null)bq = new BigQ(inPipe,outPipe,si->myOrder,si->runlength);
	}
	
	FILE* tableFile = fopen (loadpath,"r");
	Record temp;//need reference see below, make a record

	while(temp.SuckNextRecord(&f_schema,tableFile)!=0)
		inPipe->Insert(&temp);	// pipe blocks and record is consumed or is buffering required ?

	fclose(tableFile);	
}

int SortedFile::Open (char *f_path) {
	char *fName = new char[20];
	sprintf(fileHeaderName, "%s.meta", fileName);
	FILE *f = fopen(fileHeaderName,"r");
	
	// to decide what to store in meta file
	// and parse and get sort order and run length
	// requires some kind of de serialization
	// initialize it
	
	
	// set to read mode
	// bring first page into read buffer and initialize first record
	
	fclose(f);
	
	file->Open(1, fpath);	// open the corresponding file
	pageIndex = 1;
	endOfFile = 0;
}

void SortedFile::MoveFirst () {			// requires MergeFromOuputPipe()
	
	if(m==Mode.R){
		// In read mode, so direct movefirst is possible
		file->GetPage(readPageBuffer,1); //TODO: check off_t type,  void GetPage (Page *putItHere, off_t whichPage)
		readPage->GetFirst(current);
	}
	else{
		// change mode to read
		
		// Merge contents if any from BigQ
		
		// bring the first page into readPageBuffer
		// Set curr Record to first record
		// 
	}
	
}

int SortedFile::Close () {			// requires MergeFromOuputPipe()	done
	file->Close();
	endOfFile = 1;
	// write updated state to meta file
}

void SortedFile::Add (Record &rec) {	// requires BigQ instance		done
	
	if(m!=Mode.W){
		m = Mode.W;
		// create input, output pipe and BigQ instance
		if(inPipe!=null)inPipe = new Pipe();	// requires size ?
		if(outPipe!=null)outPipe = new Pipe();
		if(bq!=null)bq = new BigQ(inPipe,outPipe,si->myOrder,si->runlength);
	}
	inPipe->Insert(&rec);	// pipe blocks and record is consumed or is buffering required ?
	
}

int SortedFile::GetNext (Record &fetchme) {		// requires MergeFromOuputPipe()		done
	
	if(m!=Mode.R){
	
		m = Mode.R;
		readPageBuffer->EmptyItOut();		// requires flush
		MergeFromOutpipe();		// 
		MoveFirst();	// always start from first record
	
	}
	
	if(endofFile==1) return 0;
	while(!readPageBuffer->GetFirst(&fetchme)) {
		
		if(pageIndex>=this->file->GetLength()-1){
				endOfFile = 1;	
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
	inPipe.ShutDown();
	// get sorted records from output pipe
	CompareEngine *ce;
	
	MoveFirst();						// following four lines get the first record from those already present (not done)
	*pageToBeMerged = new Page();
	Record *rFromFile = new Record();
	rFromFile
	
	Record *rtemp = new Record();
	Page *ptowrite = new Page();
	File *newfile = new File();
	newFile->Open(0,"newFile");
	
	boolean nomore = false;
	
	while(true){
		
		if(outPipe.Remove(rtemp)==1){		// got the record from outpipe
			
			while(ce->Compare(rFromFile,rtemp,si->myOrder)<=0){ 		// merging this record with others
				if(ptowrite->Append(rFromFile)!=1){		// copy already existing record
						// page full
						int pageIndex = newFile->GetLength();
						//*
						// write this page to file
						newFile->Add(ptowrite,pageIndex);
						// empty this out
						ptowrite->EmptyItOut();
						// append the current record ?
						ptowrite->Append(rtemp);		// does this consume the record ?
				}
				
				// bring next rFromFile record ?
				// check if records already present are exhausted
				// set nomore = true; and break;
			}
			if(ptowrite->Append(rtemp)!=1){				// copy record from pipe
						// page full
						int pageIndex = newFile->GetLength();
						//*
						// write this page to file
						newFile->Add(ptowrite,pageIndex);
						// empty this out
						ptowrite->EmptyItOut();
						// append the current record ?
						ptowrite->Append(temp);		// does this consume the record ?
			}
		
		}
		else{		// pipe is empty now, done
			break;
		}
	}
	if(nomore==true){
		do{
			if(ptowrite->Append(rtemp)!=1){				// copy record from pipe
						// page full
						int pageIndex = newFile->GetLength();
						//*
						// write this page to file
						newFile->Add(ptowrite,pageIndex);
						// empty this out
						ptowrite->EmptyItOut();
						// append the current record ?
						ptowrite->Append(temp);		// does this consume the record ?
			}
		}while(outPipe.remove(rtemp)!=0);
	}
	

}

Sorted::~Sorted() {
	delete readerPageBuffer;
	delete inPipe;
	delete outPipe;
}
