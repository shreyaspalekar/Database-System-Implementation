#include "RelOp.h"

// SelectPipe

void *SelectPipe::selectPipe(void *arg) {
	SelectPipe *sp = (SelectPipe *) arg;
	sp->DoSelectPipe();
	return NULL;
}

void SelectPipe::mySelectPipe() {
	Record *tmpRecord = new Record();
	ComparisonEngine cmp;
	while(this->inPipe->Remove(tmpRecord)) {
		if(cmp.Compare(tmpRecord, this->literal, this->selOp)) {	// compares record with CNF, if true puts it into outPipe
			this->outPipe->Insert(tmpRecord);
		}
	}
	delete tmpRecord;
	this->outPipe->ShutDown();
}

void SelectPipe::Run (Pipe &inPipe, Pipe &outPipe, CNF &selOp, Record &literal){
	this->inPipe = &inPipe;
	this->outPipe = &outPipe;
	this->selOp = &selOp;
	this->literal = &literal;
	pthread_create(&thread, NULL, selectPipe, this);
}

void SelectPipe::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void SelectPipe::Use_n_Pages (int runlen) {
	this->nPages = runlen;
}

// SelectFile

void *SelectFile::selectFile(void *arg) {
	SelectFile *sp = (SelectFile *) arg;
	sp->DoSelectFile();
	return NULL;
}

void SelectFile::mySelectFile() {
	Record *tmpRecord = new Record();
	ComparisonEngine cmp;
	while(this->inFile->GetNext(*tmpRecord)) {
		if(cmp.Compare(tmpRecord, this->literal, this->selOp)) {	// compares record with CNF, if true puts it into outPipe
			this->outPipe->Insert(tmpRecord);
		}
	}
	delete tmpRecord;
	this->outPipe->ShutDown();
}

void SelectFile::Run (DBFile &inFile, Pipe &outPipe, CNF &selOp, Record &literal) {
	this->inFile = &inFile;
	this->outPipe = &outPipe;
	this->selOp = &selOp;
	this->literal = &literal;
	pthread_create(&thread, NULL, selectPipe, this);
}

void SelectFile::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void SelectFile::Use_n_Pages (int runlen) {
	this->nPages = runlen;
}


// Project 

void *Project::project(void *arg) {
	Project *pj = (Project *) arg;
	pj->DoProject();
	return NULL;
}

void Project::DoProject() {
	Record *tmpRcd = new Record;
	while(this->inPipe->Remove(tmpRcd)) {
		tmpRcd->Project(this->keepMe, this->numAttsOutput, this->numAttsInput);
		this->outPipe->Insert(tmpRcd);
	}
	this->outPipe->ShutDown();
	delete tmpRcd;
	return;
}

void Project::Run (Pipe &inPipe, Pipe &outPipe, int *keepMe,
		int numAttsInput, int numAttsOutput) {
	this->inPipe = &inPipe;
	this->outPipe = &outPipe;
	this->keepMe = keepMe;
	this->numAttsInput = numAttsInput;
	this->numAttsOutput = numAttsOutput;
	pthread_create(&thread, NULL, project, this);
}

void Project::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void Project::Use_n_Pages (int n) {
	this->nPages = n;
}

// Duplicate Removal

void DuplicateRemoval::duprem(void *arg){
	DuplicateRemoval *dr = (DuplicateRemoval *) arg;
	dr->myDuplicateRemoval();	
	return NULL:
}

void DuplicateRemoval::myDuplicateRemoval(){
	OrderMaker *om = new OrderMaker(this->mySchema);
	
	Pipe myOutPipe(1000);
	
	BigQ *bq = new BigQ(*(this->inPipe),myOutPipe,*om, );	// ? requires run length 4th parameter
	
	Record *tmpRcd = new Record();
	Record *r1 = new Record();
	
	ComparisonEngine *ce;
	
	myOutPipe->Remove(r1);				// get first record
	Record *copyMe = new Record();
	copyMe->Copy(r1);					// copy it
	this->outPipe->Insert(copyMe);		// put in out pipe
	
	while(myOutPipe->Remove(tmpRcd)){
		if(ce->Compare(r1,tmpRcd,om)!=0){	// compare next with current, if equal then continue; else put copy of next into out pipe and make current as next
			r1->Copy(tmpRcd);
			this->outPipe->Insert(tmpRcd);
		}
	}
	
	this->outPipe->ShutDown();
	myOutPipe->ShutDown();
}

void DuplicateRemoval::Run(Pipe &inPipe, Pipe &outPipe, Schema &mySchema) { 
	this->inPipe = &inPipe;
	this->outPipe = &outPipe;
	this->schema = &mySchema;
	pthread_create(&thread,NULL,duprem,this);
}

void DuplicateRemoval::WaitUntilDone () { 
	pthread_join(thread,NULL);
}

void DuplicateRemoval::Use_n_Pages (int n) { 
	this->nPages = n;
}

