#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"

// stub file .. replace it with your own DBFile.cc

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
}

void DBFile::MoveFirst () {
//USE GetNext ? is two line code better??
	this->file->GetPage(this->readPage,0); //TODO: check off_t type,  void GetPage (Page *putItHere, off_t whichPage)
	this->readPage->GetFirst(this->current);//check if this->readPage is pointer

}

int DBFile::Close () {

	//TODO:metadata

	return this->file->Close();
	
}

void DBFile::Add (Record &rec) {

	//Consume rec
	Record write;
	write.Consume(&rec);//consume needs a pointer, is this right?
	
	if(writePage->Append(&write)==0)
	{		
		this->file->AddPage(writePage,(this->file->GetLength())+1);
		this->writePage->EmptyItOut();
		writePage->Append(&write);
	}
		
}

int DBFile::GetNext (Record &fetchme) {
	
	//BUG: Might return wrong results
	
	fetchme.Consume(*current);
	
	int result = this->readPage->GetFirst(this->current);
	

	if(result==0){//int GetFirst (Record *firstOne), check type
		
		pageIndex++;
		
		if(pageIndex>this->file->GetLength())
			return 0;
		
		else{
			this->file->GetPage(this->readPage,pageIndex);
			this->readPage->GetFirst(this->current);
		}
		
		
	}
	
	
	
	//is this right? REFERENCES CANNOT BE REINITIALIZED
	return 1;
	
}
//TODO: where will the current pointer be at the end??
int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {

	ComparisonEngine compare;
	int result1 = 0;
	int result2 = 1;
	
	while(result1==0||result2!=0){
		
		result2 = this->GetNext(*current);//requires reference..check if is right
		result1 = compare.Compare(this->current,&literal,&cnf);//int Compare(Record *left, Record *literal, CNF *myComparison); is this right
	
	}
	
	if(result1==1)
	{
		fetchme = *current;
		return 1;
	}
	
	return 0;
}
