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
	//Page buffer of size runlen to store read in records

	
	Record temp;
	Record *runBuff;
	Page *buffer;
	File *runs;
	int no_runs;
	pthread_t worker;
	int page_Index;
	
	struct args_phase1_struct {                                                   
		//check initialization
		Pipe *input;
		OrderMaker *sort_order;
		int run_length;
		Record *runBuffer = runBuff; 
		int &num_runs = no_runs;
		Record *temporary = &temp;
		Page *buf = buffer;
		File *run_buffer = runs;
		char *file_path = "run";
	
	}args_phase1;

	void* TPMMS_Phase1(void* arg);
	void* TPMMS_Phase2(void* arg);

public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
};

#endif
