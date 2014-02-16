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
	char *f_name;
	
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
//	static bool sort_func(Record &,Record &,OrderMaker &sortorder);	
	/*Deprecated: Replaced by DBFile , no need for indexing
	//Record *recordBuff;
	//int pageLength = 0;//no of recrods per page
	//Page *buffer;*/
	

public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
};

class sort_func{

private:

	OrderMaker *sort_order;

public:

	sort_func(OrderMaker *order){
		this->sort_order = order;
	}

	sort_func(){};

	bool operator()(const Record &one,const Record &two){

		/*Record *one = new Record();

		Record *two = new Record();
		one->Copy(const_cast<Record *>(&onei));		

		


		two->Copy(const_cast<Record *>(&twoi));		
		*/	
		Record *inp1 = const_cast<Record*>(&one);	
		Record *inp2 = const_cast<Record*>(&two);	

//		cout<<"Read at location"<<inp1;
		cout<<"Comparing "<<inp1<<" and "<<inp2<<"\n";

		ComparisonEngine *compare;

		if(compare->Compare(inp1,inp2,this->sort_order)<=0)
			return true;


		else
			return false;

	}

	

};

#endif
