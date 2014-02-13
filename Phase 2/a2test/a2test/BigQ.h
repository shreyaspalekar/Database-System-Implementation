#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"

using namespace std;

class BigQ {

	//DataStructures 
	//Record temp to read in records from pipes
	
	Record temp;
	DBFile *runs;
	int no_runs;
	pthread_t worker;
	int page_Index;
	char *f_name = "run";
	
	struct args_phase1_struct {                                                   
		
		//!!check initialization
		
		Pipe *input;
		OrderMaker *sort_order;
		int run_length;
		int &num_runs = no_runs;
		Record *temporary = &temp;
		DBFile *run_buffer = runs;
		char *file_path = f_name;
		
		/*Deprecated: Replaced by DBFile , no need for indexing
		//int pageLen = pageLength;
		//Record *recordBuffer = recordBuff; 
		//Page *buf = buffer;*/
	
	}args_phase1;

	void* TPMMS_Phase1(void* arg);
	void* TPMMS_Phase2(void* arg);
	
	/*Deprecated: Replaced by DBFile , no need for indexing
	//Record *recordBuff;
	//int pageLength = 0;//no of recrods per page
	//Page *buffer;*/
	

public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
};

#endif
