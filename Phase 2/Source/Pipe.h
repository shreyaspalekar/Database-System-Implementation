#ifndef PIPE_H
#define PIPE_H

#include <pthread.h>

#include "Record.h"


class Pipe {
private:

	// these are used for data storage in the pipeline
	Record *buffered;

	int firstSlot;
	int lastSlot;
	int totSpace;

	int done;

	// mutex for the pipe
	pthread_mutex_t pipeMutex;

	// condition variables that the producer and consumer wait on
	pthread_cond_t producerVar;
	pthread_cond_t consumerVar;

public:

	// this sets up the pipeline; the parameter is the number of
	// records to buffer
	Pipe (int bufferSize);	
	virtual ~Pipe();

	// This inserts a record into the pipeline; note that if the
	// buffer size is exceeded, then the insertion may block
	// Note that the parameter is consumed; after insertion, it can
	// no longer be used and will be zero'ed out
	void Insert (Record *insertMe);

	// This removes a record from the pipeline and puts it into the
	// argument.  Note that whatever was in the parameter before the
	// call will be lost.  This may block if there are no records in
	// the pipeline to be removed.  The return value is a 1 on success
	// and a zero if there are no more records in the pipeline
	int Remove (Record *removeMe);

	// shut down the pipepine; used by the consumer to signal that 
	// there is no more data that is going to be added into the pipe
	void ShutDown ();

};

#endif
