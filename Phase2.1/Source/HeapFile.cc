#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include "HeapFile.h"
// stub file .. replace it with your own Heap.cc

//TODO:optimise for branch misses

HeapFile::HeapFile () {

	this->file = new File();
	this->readPage = new Page();
	this->writePage = new Page();
	this->current = new Record();

}

HeapFile::~HeapFile(){
	delete file;
	delete readPage;
	delete writePage;
	delete current;

}

int HeapFile::Create (char *f_path, fType f_type, void *startup) {

	*(this->file_path) = *f_path;
	this->file->Open(0,f_path);
	pageIndex=1;
	writeIndex=1;
	writeIsDirty=0;
	endOfFile=1;

	return 1;//TODO:check>> void for File Open !!
}

void HeapFile::Load (Schema &f_schema, char *loadpath) {

	FILE* tableFile = fopen (loadpath,"r");
	Record temp;//need reference see below, make a record

	while(temp.SuckNextRecord(&f_schema,tableFile)!=0)
		this->Add(temp);//TODO: add requires a reference

	fclose(tableFile);
}

int HeapFile::Open (char *f_path) {
	//TODO:metadata
	*(this->file_path) = *f_path;
	this->file->Open(1,f_path);
	pageIndex=1;
	endOfFile = 0;
	return 1;
}

void HeapFile::MoveFirst () {
//USE GetNext ? is two line code better??
//Should we let File.cc handle the boundry conditions??
	//if(this->file->GetLength()>1){
		this->file->GetPage(this->readPage,1); //TODO: check off_t type,  void GetPage (Page *putItHere, off_t whichPage)
		this->readPage->GetFirst(this->current);//check if this->readPage is pointer
	//}
}

int HeapFile::Close () {

	if(this->writeIsDirty==1){
		this->file->AddPage(writePage,writeIndex);
		writeIndex++;
	}

	//metadata
	char meta_path[20];

	sprintf(meta_path,"%s.meta",this->file_path);

	FILE *meta =  fopen(meta_path,"w");

	fprintf(meta,"%s","heap");
	//metadata


	endOfFile = 1;
	return this->file->Close();

}

void HeapFile::Add (Record &rec) {

	//Consume rec
	this->writeIsDirty=1;

	Record write;
	write.Consume(&rec);//consume needs a pointer, is this right?

	if(writePage->Append(&write)==0)
	{		
		this->file->AddPage(writePage,writeIndex);
		writeIndex++;
		this->writePage->EmptyItOut();
		writePage->Append(&write);
	}

}

int HeapFile::GetNext (Record &fetchme) {

	//BUG: Might return wrong results
	//Fix structure return 0 may not be called
	if(endOfFile!=1){


		fetchme.Copy(this->current);

		int result = this->readPage->GetFirst(this->current);


		if(result==0){//int GetFirst (Record *firstOne), check type

			pageIndex++;

			if(pageIndex>=this->file->GetLength()-1){
				endOfFile = 1;	
			}

			else{
				this->file->GetPage(this->readPage,pageIndex);
				this->readPage->GetFirst(this->current);
			}


		}



	//is this right? REFERENCES CANNOT BE REINITIALIZED
		return 1;
	}
	return 0;

}
//TODO: where will the current pointer be at the end??
int HeapFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {

	ComparisonEngine compare;
	int result1 = 0;
	int result2 = 1;
	//Record temp2;

	while(result1==0&&result2!=0){

		result2 = this->GetNext(fetchme);//requires reference..check if is right
		result1 = compare.Compare(&fetchme,&literal,&cnf);//int Compare(Record *left, Record *literal, CNF *myComparison); is this right

	}

	if(result2==0)
		return 0;

	if(result1==1)
		return 1;


	return 0;
}
