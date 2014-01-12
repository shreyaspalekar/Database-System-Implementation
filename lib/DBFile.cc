#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"

// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {}

int DBFile::Create (char *f_path, fType f_type, void *startup) {

	this.File->Open(0,fpath);
	pageIndex=1;

}

void DBFile::Load (Schema &f_schema, char *loadpath) {
	
	FILE *tableFile = fopen (loadpath,"r");
	Record temp;
	
	while(temp->SuckNextRecord(f_schema,tableFile)!=0)
		this.Add(temp);
		
	fclose(tableFile);
}

int DBFile::Open (char *f_path) {
	//TODO:metadata
	this.File->Open(1,fpath);
	pageIndex=1;
}

void DBFile::MoveFirst () {
//USE GetNext ? is two line code better??
	this.file->GetPage(this.readPage,0); //TODO: check off_t type
	this.readPage->GetFirst(this.current);

}

int DBFile::Close () {

	//TODO:metadata

	return this.file->close();
	
}

void DBFile::Add (Record &rec) {

	//Consume rec
	this.write->consume(rec);
	
	if(writePage->append(write)==0)
	{		
		this.file->AddPage(writePage,this.file->GetLength+1);
		this.writePage->EmptyItOut();
		writePage->append(write);
	}
		
}

int DBFile::GetNext (Record &fetchme) {
	

	if(this.readPage->GetFirst(this.current)==0){
		
		pageIndex++;
		
		if(pageIndex>this.file->GetLength())
			return 0;
		
		else{
			this.file->GetPage(this.readPage,pageIndex);
			this.readPage->GetFirst(this.current);
		}
		
		
	}
	
	
	
	fetchme = this.current;
	return 1;
	
}
//TODO: where will the current pointer be at the end??
int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {

	ComparisonEngine compare;
	int result1 = 0;
	int result2 = 1;
	
	while(result1==0||result2!=0){
		
		result2 = this.GetNext(this.read)
		result1 = compare.Compare(this.read,literal,cnf);
	
	}
	
	if(result1==1)
	{
		fetchme = this.read;
		return 1;
	}
	
	return 0;
}
