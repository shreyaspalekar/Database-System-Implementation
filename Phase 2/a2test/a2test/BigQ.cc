#include "BigQ.h"
#include <algorithm>

void* BigQ::TPMMS_Phase1(void* arg){

	/*Typecast the arguments
	*/
	args_phase1_struct *args;
	args = (args_phase1_struct *)arg;

	*(args->num_runs) = -1;//goes from 0 to n,set to one as the array size is n, else set array size to n+1 to use indexing from 1
	
	//Create new DBFile object
	DBFile *new_file = new DBFile();
	*(args->num_runs)++;
	(args->run_buffer)->push_back(new_file);//file for run1
	
	//Create new binary file by name run.run_no
	char actual_path[20];
	sprintf(actual_path,"%s.%d","run",*(args->num_runs));
	args->run_buffer->at(*(args->num_runs))->Create(actual_path,heap,NULL);

	cout << "Created run file "<<*(args->num_runs)<<"\n";
	
	int result =1;
	int num_recs =0;

	//***check resets of indexes
	while(result!=0){ // till input pipe is empty
	
				
		//Read record from pipe
	        result = args->input->Remove(args->temporary); // till input pipe is empty
								//append record temporary to page at pageindex
		//Increment record counter
		num_recs++;		

		if(((int)args->run_buffer->at(*(args->num_runs))->GetLength()<*(args->run_length))&&result!=0){

			args->run_buffer->at(*(args->num_runs))->Add(*(args->temporary));

		}

		else{
			
		        cout<<"Pages read "<<(int)args->run_buffer->at(*(args->num_runs))->GetLength()<<"\n\n";
	                cout << "Run No:  "<<*(args->num_runs)<<"\n";
	                
	                /*TODO: we should not close and open the file again*/

			args->run_buffer->at(*(args->num_runs))->Close();
			
			args->run_buffer->at(*(args->num_runs))->Open(actual_path);
			
			args->run_buffer->at(*(args->num_runs))->MoveFirst();
			
			cout << "Close and open file\n";
			
			int z = 0;

			//Empty file into record buffer

    	      		Record **record_Buffer= new Record*[num_recs];
			Record *temp = new Record();
			int count =0;
			
			while(args->run_buffer->at(*(args->num_runs))->GetNext(*temp) != 0){//empty out file into vector
				
				record_Buffer[count] = new Record();
				record_Buffer[count++]->Consume(temp);
				temp = new Record();
				z++;
			}	
			
			
			cout << "read "<<z<<" records\n";
			cout << "Emptied file into record buffer\n";
			cout << "Record Buffer size: "<<count<<"\n";
			//cout << "Record Buffer Before  "<<record_Buffer;
			sort(record_Buffer,record_Buffer+( sizeof record_Buffer / sizeof record_Buffer[0]),sort_func(args->sort_order));	

			cout << "Sorted Record buffer\n";
			//cout << "Record Buffer After"<< record_Buffer;

			//Reset file 
			args->run_buffer->at(*(args->num_runs)) = new DBFile();
						
			args->run_buffer->at(*(args->num_runs))->Create(actual_path,heap,NULL);

			//Empty record buffer back into file
			for(int i=0;i<( sizeof record_Buffer / sizeof record_Buffer[0]);i++){//empty record buffer into dbfile
				
				cout << record_Buffer[i];

			
				Record *temp = new Record();
				temp->Copy(record_Buffer[i]);//check if this copies
				args->run_buffer->at(*(args->num_runs))->Add(*temp);//check for references and pointers DOES THIS PERFORM DEEP COPY??
			}	

			//close dbfile
			cout << "Emptied Record Buffer\n";
			args->run_buffer->at(*(args->num_runs))->Close();

			//new File
			cout << "Closed file\n";
			
                        DBFile *new_file = new DBFile();
	                *(args->num_runs)+=1;
			args->run_buffer->push_back(new_file); //create new run file

			cout<<"num_runs "<<*(args->num_runs)<<" vector Size "<<args->run_buffer->size();
		
			sprintf(actual_path,"%s.%d","run",*(args->num_runs));//set path as "file_path.num_run"
			args->run_buffer->at(*(args->num_runs))->Create(actual_path,heap,NULL);//??concatenate run no
			args->run_buffer->at(*(args->num_runs))->Add(*(args->temporary));		
			cout << "Created new file\n\n";

		}
		
		//  if page is full, increment page index
		//  if page index is out of bound, sort() page[]
		// write out as page[] run as binary
		// empty put page[]
		
	}
	cout << "Closing last file";	
//WARNING	args->run_buffer->at(*(args->num_runs))->Close();

	//Open all files for merging	
	for (int i=0;i<args->run_buffer->size();i++){
		char path[20];
		sprintf(path,"%s.%d","run",i);//(args->file_path) removed
	
		args->run_buffer->at(i)->Open(path);
		args->run_buffer->at(i)->MoveFirst();

		cout<<"Opened run "<<i<<" \n";
	}

	cout<<"All files opened \n";
	priority_queue<Record* , vector<Record*> , sort_func> pQueue (sort_func(args->sort_order));

	//build priority queue

	Record *temp = new Record();
	
	for (int i=0;i<args->run_buffer->size();i++){
		
		args->run_buffer->at(i)->GetNext(*temp);
		pQueue.push(temp);	
		temp = new Record();
	}
    	
	int flags=args->run_buffer->size();/*[args->run_buffer->size()];

	for

	 for (i=0;i<args->run_buffer->size();i++){
	
		flags[i]=1;
	}*/
	Record **next = new Record*[args->run_buffer->size()];
	
	for (int i=0;i<args->run_buffer->size();i++){

                        args->run_buffer->at(i)->GetNext(*(*(next+i)));

        }



	while(flags!=0){

		Record *temp = new Record();
		temp = pQueue.top();
		pQueue.pop();
		args->output->Insert(temp);
		ComparisonEngine *compare;
		Record *insert = new Record();
		insert->Copy(*(next+0));
		int min =0;

		for (int i=1;i<args->run_buffer->size();i++){
		
			if(compare->Compare(insert,*(next+i),args->sort_order) <=0 ){ 
				insert->Copy(*(next+i));
				min =i;
			}
		}

		if(args->run_buffer->at(min)->GetNext(*(*(next+min)))==0)
		{	flags--;
			args->run_buffer->erase(args->run_buffer->begin()+min);
		}

		pQueue.push(insert);
	
	} 
}



BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	
	this->no_runs = 0;
	
	
	//buffer = new Page[runlen];//set to runlen +1 to use indxing starting from 1
	//runBuff = new Record[pageLength];//how many records per page?
	runs = new vector<DBFile*>();
//	*f_name = 'run';

	args_phase1.num_runs = &no_runs;
	args_phase1.temporary = &temp;
	args_phase1.run_buffer = runs;
	args_phase1.file_path = "run";
	args_phase1.input = &in;
	args_phase1.output = &out;
	args_phase1.sort_order = &sortorder;
	args_phase1.run_length = &runlen;
	
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
