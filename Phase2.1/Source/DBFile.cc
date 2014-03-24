#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include "TwoWayList.h"
#include "Record.h"
#include "DBFile.h"
#include <string.h>


// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {

}

int DBFile::Create (char *f_path, fType f_type, void *startup) {

	if(f_type==heap){

		this->file= new HeapFile();

	}
	else if(f_type==sorted){

		this->file= new SortedFile();	

	}

	if(file!=NULL){

		file->Create(f_path,f_type,startup);		

	}	

}

void DBFile::Load (Schema &f_schema, char *loadpath) {

	file->Load(f_schema,loadpath);

}

int DBFile::Open (char *f_path) {

	char meta_path[20];

	sprintf(meta_path,"%s.meta",f_path);

	FILE *meta =  fopen(meta_path,"r");;
	char f_type[10];

	fscanf(meta,"%s",f_type);

	//cout<<"scanned file type"<<endl;
	//cout<<f_type<<endl;
	//cout<<strcmp(f_type,"sorted")<<endl;

	//read meta details from file

	if(strcmp(f_type,"heap")==0){
	

		//cout<<"created heap file";
		HeapFile *h = new HeapFile();
		this->file= h;

	}
	else if(strcmp(f_type,"sorted")==0){
	

		//cout<<"created sorted file"<<endl;
		SortedFile *s = new SortedFile();
		this->file= s;	

	}

	//cout<<file<<endl;
	file->Open(f_path);

	//Note:should we read the order maker at this level or let the sorted file handle it??



}

void DBFile::MoveFirst () {

	file->MoveFirst();

}

int DBFile::Close () {

	return file->Close();

}

void DBFile::Add (Record &rec) {

	return file->Add(rec);

}

int DBFile::GetNext (Record &fetchme) {

	return file->GetNext(fetchme);

}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {

	return file->GetNext(fetchme,cnf,literal);

}
