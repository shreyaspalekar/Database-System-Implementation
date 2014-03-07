#include "BigQ.h"
#include <algorithm>

void BigQ::sort_run(Page *p, int num_recs,File& new_file,int& gp_index,OrderMaker *sort_order){


	cout<<"G index start "<<gp_index<<"\n";


	Record **record_Buffer= new Record*[num_recs];

	int c   =  0;
	int p_i =  0;
				

	do{	


		record_Buffer[c] = new Record();
				
				
		if((p+p_i)->GetFirst(record_Buffer[c])==0){
						
			(p+p_i)->EmptyItOut();	
			p_i++;
			(p+p_i)->GetFirst(record_Buffer[c]);
			c++;

		}
		else{
			c++;
		}
		

	}while(c < num_recs);

	(p+p_i)->EmptyItOut();

	sort(record_Buffer,record_Buffer+(num_recs),sort_func(sort_order));

	//DEBUG

	int k = 0,err = 0,succ = 0;


	Record *last = NULL, *prev = NULL;


	ComparisonEngine ceng;
	while (k < num_recs) {


		prev = last;


		last = record_Buffer[k];






		if (prev && last) {


			if (ceng.Compare (prev, last, sort_order) == 1) {


				err++;
			}


			else {
				succ++;


			}
		}


		k++;
	}



	cout << "ERROR in sorting this run = " << err << endl;
	cout << "succ in sorting this run = " << succ << endl;


	

	/*Schema schema("catalog","lineitem");
	for(int i=0;i<num_recs;i++){
	
		(*(record_Buffer+i))->Print(&schema);

	}*/
	//DEBUG


	c=0;

	Page *tp = new Page();
	int pageIsDirty =0;
	//Page tp;		
	
	while(c < num_recs){
					
		if(tp->Append(*(record_Buffer+c))==0){

			pageIsDirty =0;
			new_file.AddPage(tp,(off_t)(gp_index++));
			tp->EmptyItOut();
			tp->Append(*(record_Buffer+c));
			c++;	

		}
		else{

			pageIsDirty =1;
			c++;

		}						



	}
		
	if(pageIsDirty==1){

		new_file.AddPage(tp,(off_t)(gp_index++));	

	}

	cout<<"G index end "<<gp_index<<"\n";

	delete tp;
	
	for(int i=0;i<num_recs;i++){
		delete *(record_Buffer+i);
	}

	delete record_Buffer;

}


void* BigQ::TPMMS_Phase1(void* arg){

	/*Typecast the arguments
	*/
	args_phase1_struct *args;
	args = (args_phase1_struct *)arg;

	//Create new DBFile object
	File new_file;

	char f_path[8] = "runfile";
	new_file.Open(0,f_path);

	
	Page *p = new Page[*(args->run_length)]();	
 
	Record *temporary = new Record();//check mem

//	cout << "Created run file "<<*(args->num_runs)<<"\n";
	
	int result    =  1;		//integer boolean for checking pipe status
	int num_recs  =  0;		//no of records 
	int p_index   =  0;		//pageindex to write at
	int gp_index   =  1;		//global page index for file
	//int r_index[*(args->run_length)];	
	int num_runs  =  1;		//goes from 1 to n,set to one as the array size is n, else set array size to n+1 to use indexing from 1
	//r_index[num_runs-1]=1;
	

	//***check resets of indexes
	while(result!=0){ // till input pipe is empty
	
				
		//Read record from pipe
	        result = args->input->Remove(temporary); // till input pipe is empty
								//append record temporary to page at pageindex
		//Increment record counter

		if(result!=0){
	
			if((p+p_index)->Append(temporary)==1){
	
				num_recs++;
				//temporary = new Record();	VALGRIND	

			}

			else if(++p_index == *(args->run_length)){

				cout <<"Run no "<<num_runs<<"\n";
				cout <<"Pages "<<p_index<<"\n";
				cout <<"No of Records "<<num_recs<<"\n\n";				

				sort_run(p,num_recs,new_file,gp_index,args->sort_order);			
				
				//r_index[num_runs++] = p_index;
				num_runs++;

				p_index=0;
				num_recs=0;
				
				(p+p_index)->Append(temporary);
				num_recs++;				
				temporary = new Record();

				

			}

			else{
		
				(p+p_index)->Append(temporary);
				num_recs++;
				temporary = new Record();
			}
		}

		else{

			cout <<"Run no "<<num_runs<<"\n";
			cout <<"Pages "<<p_index<<"\n";
			cout <<"No of Records "<<num_recs<<"\n\n";		

			
			sort_run(p,num_recs,new_file,gp_index,args->sort_order);
			
	                cout << "Runs created  " << num_runs << "\n";
			cout << "Total Pages Read " <<  gp_index << "\n";
			

			//Empty file into record buffer

    	  			
			//DO NOT COMMENT
			
			//Schema schema("catalog","lineitem");
			//cout<<"Count: "<<count<<"";
			/*for(int i=0;i<count;i++){
				cout<<"Printing record: "<<i<<" Count: "<<count<<"\n";
				(*(record_Buffer+i))->Print(&schema);
			}*/


		}
		

	}

//	cout<<"All files opened \n";

	delete temporary;
	//delete p; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!WHYYY?????

	//DEBUG
	new_file.Close();
	new_file.Open(1,f_path);
	//DEBUG



	priority_queue<rwrap* , vector<rwrap*> , sort_func> pQueue (sort_func(args->sort_order));

	//build priority queue

	Page *buf = new Page[num_runs];

	rwrap *temp = new rwrap;
	Record *t = new Record();	

	for (int i=0;i<num_runs;i++){
		
		new_file.GetPage((buf+i),(off_t)(1+(*args->run_length)*i));
		(buf+i)->GetFirst(t);
		(temp->rec).Consume(t);
		temp->run=i;		
		pQueue.push(temp);	
		t = new Record();
		temp = new rwrap;
	}
    	

	
//	Schema schema("catalog","lineitem");
//	(pQueue.top())->Print(&schema);
		
	cout<<"Setting Indexes\n";	

	int flags  = num_runs;
	int next  = 0;
	int start = 0;
	int end   = 1;
	int fin[num_runs];//set to 0
	int c_i[num_runs];	
	int index[num_runs][2];


	for(int i=0;i<num_runs;i++){
	
		fin[i]=0;
		c_i[i]=0;

	}
	
	for(int i=0;i<num_runs-1;i++){

		index[i][start] = 1+((*(args->run_length))*i);
		index[i][end]   = (*(args->run_length))*(i+1);
	}

	index[num_runs-1][start] = 1+((*(args->run_length))*(num_runs-1));
	index[num_runs-1][end] = gp_index-1;

	//DEBUG
		
	for(int i=0;i<num_runs;i++){

		cout<<"run "<<i<<" start "<<index[i][start]<<" end "<<index[i][end]<<"\n";	
	
	}	


	//DEBUG

	cout<<"Begin Merge\n";
	while(flags!=0){

		rwrap *temp;
		temp = pQueue.top();
		pQueue.pop();

		next = temp->run;

		args->output->Insert(&(temp->rec));

		rwrap *insert = new rwrap;		
		Record *t = new Record();
 
		if(fin[next]==0)
		{
			if((buf+next)->GetFirst(t)==0){
			
				if( index[next][start]+ (++c_i[next]) > index[next][end]){
				//what do you insert?
					flags--;
					fin[next]=1;				
				}
				else{

					cout<<"Read at index"<<index[next][start]+c_i[next]<<"\n";

					new_file.GetPage(buf+next,(off_t)(index[next][start]+c_i[next]));

					(buf+next)->GetFirst(t);
					insert->rec = *t;
					insert->run = next; 	
					pQueue.push(insert);
					
				}

			}
			else{

				insert->rec = *t;
				insert->run = next; 	
				pQueue.push(insert);

			}	
		}	

		
		//delete insert;   	DOUBLE FREE ERROR
		//delete t;		DOUBLE FREE ERROR
	
	
	}

	while(!pQueue.empty()){
		args->output->Insert(&((pQueue.top())->rec));
		pQueue.pop();
	} 
}



BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	
	this->no_runs = 0;
	
	args_phase1.input = &in;
	args_phase1.output = &out;
	args_phase1.sort_order = &sortorder;
	args_phase1.run_length = &runlen;
	
	sortorder.Print();
	
	pthread_create (&worker, NULL, &BigQ::TPMMS_Phase1 , (void *)&args_phase1);

	pthread_join(worker,NULL);
	
	// read data from in pipe sort them into runlen pages
	
    	// construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    	// finally shut down the out pipe
	out.ShutDown ();
}




BigQ::~BigQ () {
	//delete buffer;
	//delete thread
}
