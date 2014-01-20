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
	Page *buffer;
	pthread_t worker;
	int page_Index;
	
	struct args_phase1_struct {                                                   
		
		Pipe *input;
		OrderMaker *sort_order;
		int run_length;
		Record *temporary = &temp;
		Page *buf = buffer;
	
	}args_phase1;

	void* TPMMS_Phase1(void* arg);
	void* TPMMS_Phase2(void* arg);

public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
};

#endif
