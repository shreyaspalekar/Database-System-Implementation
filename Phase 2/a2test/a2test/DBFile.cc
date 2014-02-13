#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"

// stub file .. replace it with your own DBFile.cc

//TODO:optimise for branch misses

DBFile::DBFile () {

	this->file = new File();
	this->readPage = new Page();
	this->writePage = new Page();
	this->current = new Record();
	
}

DBFile::~DBFile(){
	delete file;
	delete readPage;
	delete writePage;
	delete current;

}

int DBFile::Create (char *f_path, fType f_type, void *startup) {

	this->file->Open(0,f_path);
	pageIndex=1;
	writeIndex=1;
	writeIsDirty=0;
	endOfFile=1;
	
	return 1;//TODO:check>> void for File Open !!

	
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
	
	FILE* tableFile = fopen (loadpath,"r");
	Record temp;//need reference see below, make a record
	
	while(temp.SuckNextRecord(&f_schema,tableFile)!=0)
		this->Add(temp);//TODO: add requires a reference
		
	fclose(tableFile);
}

int DBFile::Open (char *f_path) {
	//TODO:metadata
	this->file->Open(1,f_path);
	pageIndex=1;
	endOfFile = 0;
	return 1;
}

void DBFile::MoveFirst () {
//USE GetNext ? is two line code better??
//Should we let File.cc handle the boundry conditions??
	//if(this->file->GetLength()>1){
		this->file->GetPage(this->readPage,1); //TODO: check off_t type,  void GetPage (Page *putItHere, off_t whichPage)
		this->readPage->GetFirst(this->current);//check if this->readPage is pointer
	//}
}

int DBFile::Close () {

	if(this->writeIsDirty==1){
		this->file->AddPage(writePage,writeIndex);
		writeIndex++;
	}

	//TODO:metadata
	endOfFile = 1;
	return this->file->Close();
	
}

void DBFile::Add (Record &rec) {

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

int DBFile::GetNext (Record &fetchme) {
	
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
int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {

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
