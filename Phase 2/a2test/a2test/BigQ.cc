#include "BigQ.h"

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	
	buffer = new Page[runlen];
	
 	
	args_phase1.input = &in;
	args_phase1.sort_order = &sortorder;
	args_phase1.run_length = &runlen;
	
	pthread_create (&worker, NULL, TPMMS_phase1 , (void *)&args_phase1);
	
	// read data from in pipe sort them into runlen pages
	
    // construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    // finally shut down the out pipe
	out.ShutDown ();
}

BigQ::void* TPMMS_Phase1(void* arg){
	/*
	Pipe *input;
		OrderMaker *sort_order;
		int run_length;
		Record *temporary = &temp;
		Page *buf = buffer;
		*/
	int page_Index = 0;
	 struct args_phase1_struct *args = arg;
	 
	 
	 while(args->input->remove(args->temporary)!=0){ // till input pipe is empty
		
		//append record temporary to page at pageindex
		if(args->(*buf)[page_Index]->append(args->temporary) == 0){ 
			if(++page_Index>args->run_length){} // page index out of bound	
		}
		
		//  if page is full, increment page index
		//  if page index is out of bound, sort() page[]
		// write out as page[] run as binary
		// empty put page[]
		
	 }
	 
}

BigQ::~BigQ () {
	delete buffer;
	//delete thread
}
