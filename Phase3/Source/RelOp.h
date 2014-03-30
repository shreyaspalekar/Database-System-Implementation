#ifndef REL_OP_H
#define REL_OP_H

#include "Pipe.h"
#include "DBFile.h"
#include "Record.h"
#include "BigQ.h"
#include "Function.h"
#include <sstream>
#include <stdlib.h>
#include <vector>

class RelationalOp {
	public:
	// blocks the caller until the particular relational operator 
	// has run to completion
	virtual void WaitUntilDone () = 0;

	// tell us how much internal memory the operation can use
	virtual void Use_n_Pages (int n) = 0;
};

class SelectFile : public RelationalOp { 

	static void *selectFile(void *);
	
	private:
		pthread_t thread;  //the thread that run method will spawn
		int nPages; 
		DBFile *inFile;
		Pipe *outPipe;
		CNF *selOp;
		Record *literal;
		void DoSelectFile();
		
	public:

		void Run (DBFile &inFile, Pipe &outPipe, CNF &selOp, Record &literal);
		void WaitUntilDone ();
		void Use_n_Pages (int n);

};

class SelectPipe : public RelationalOp {	// streaming operation
	
	static void *selectPipe(void *);
	
	private:
		pthread_t thread;  //the thread that run method will spawn
		int nPages;
		Pipe *inPipe, *outPipe;
		CNF *selOp;
		Record *literal;
		void DoSelectPipe();
		
	public:
		void Run (Pipe &inPipe, Pipe &outPipe, CNF &selOp, Record &literal);
		void WaitUntilDone ();
		void Use_n_Pages (int n);

};

class Project : public RelationalOp { 
	
	static void *project(void *);
	
	private:
		pthread_t thread;  //the thread that run method will spawn
		int nPages; //the buffer that the operator can use
		Pipe *inPipe, *outPipe;
		int *keepMe;
		int numAttsInput, numAttsOutput;
		void DoProject();
	
	public:
		void Run (Pipe &inPipe, Pipe &outPipe, int *keepMe, int numAttsInput, int numAttsOutput) ;
		void WaitUntilDone () ;
		void Use_n_Pages (int n) ;
};

class DuplicateRemoval : public RelationalOp {
	
	static void *duprem(void *);
	
	private:
		pthread_t thread;  //the thread that run method will spawn
		int nPages; //the buffer that the operator can use
		Pipe *inPipe, *outPipe;
		Schema *mySchema;
		void DoDuplicateRemoval();
	public:
		void Run (Pipe &inPipe, Pipe &outPipe, Schema &mySchema) ;
		void WaitUntilDone () ;
		void Use_n_Pages (int n) ;
};



class Join : public RelationalOp { 

	private:
	pthread_t thread;
	Pipe *inPipeL;
	Pipe *inPipeR;
	Pipe *outPipe;
	CNF *selOp;
	Record *literal;
	int nPages;

	public:
	void Run (Pipe &inPipeL, Pipe &inPipeR, Pipe &outPipe, CNF &selOp, Record &literal);
	static void* jswpn(void *);
	void join();
	void WaitUntilDone () ;
	void Use_n_Pages (int n);
};

class Sum : public RelationalOp {

	private:
	pthread_t thread;
        Pipe *inPipe;
        Pipe *outPipe;
        Function *function;
	int nPages;

	public:
	void Run (Pipe &inPipe, Pipe &outPipe, Function &computeMe);
	static void* sspwn(void*);
	void doSum();
	void WaitUntilDone () ;
	void Use_n_Pages (int n) ;
};

class GroupBy : public RelationalOp {

	private:
	pthread_t thread;
	Pipe *inPipe;
	Pipe *outPipe;
        OrderMaker *groupbyOrder;
        Function *function;
        int nPages;

	public:
	static void* gspwn(void*);
	void doGroupBy();
	void Run (Pipe &inPipe, Pipe &outPipe, OrderMaker &groupAtts, Function &computeMe);
	void WaitUntilDone () ;
	void Use_n_Pages (int n);
};


class WriteOut : public RelationalOp {
	pthread_t thread;
	public:
	void Run (Pipe &inPipe, FILE *outFile, Schema &mySchema) ;
	void WaitUntilDone ();
	void Use_n_Pages (int n) ;
};
#endif
