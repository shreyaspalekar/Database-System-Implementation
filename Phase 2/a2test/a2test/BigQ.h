#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"
#include "DBFile.h"
#include "ComparisonEngine.h"
#include <vector>

using namespace std;

class BigQ {

	//DataStructures 
	//Record temp to read in records from pipes
	
	Record temp;
	vector<DBFile*> *runs;
	int no_runs;
	pthread_t worker;
	int page_Index;
	char *f_name = "run";
	
	struct args_phase1_struct {                                                   
		
		//!!check initialization
		
		Pipe *input;
		OrderMaker *sort_order;
		int *run_length;
		int *num_runs;
		Record *temporary;
		vector<DBFile*> *run_buffer;
		char *file_path;
		
		/*Deprecated: Replaced by DBFile , no need for indexing
		//int pageLen = pageLength;
		//Record *recordBuffer = recordBuff; 
		//Page *buf = buffer;*/
	
	}args_phase1;

	typedef struct args_phase1_struct args_phase1_struct;

	static void* TPMMS_Phase1(void* arg);
	void* TPMMS_Phase2(void* arg);
	static void quicksort(vector<Record> &rb, int left, int right,OrderMaker &sortorder);
	
	/*Deprecated: Replaced by DBFile , no need for indexing
	//Record *recordBuff;
	//int pageLength = 0;//no of recrods per page
	//Page *buffer;*/
	

public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
};

#endif
