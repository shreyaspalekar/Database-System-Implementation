#include "File.h"
#include "TwoWayList.cc"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>



Page :: Page () {
	curSizeInBytes = sizeof (int);
	numRecs = 0;

	myRecs = new (std::nothrow) TwoWayList<Record>;
	if (myRecs == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}
}

Page :: ~Page () {
	delete myRecs;
}


void Page :: EmptyItOut () {

	// get rid of all of the records
	while (1) {
		Record temp;
		if (!GetFirst (&temp))
			break;
	}	

	// reset the page size
	curSizeInBytes = sizeof (int);
	numRecs = 0;
}


int Page :: GetFirst (Record *firstOne) {

	// move to the first record
	myRecs->MoveToStart ();

	// make sure there is data 
	if (!myRecs->RightLength ()) {
		return 0;
	}

	// and remove it
	myRecs->Remove (firstOne);
	numRecs--;

	char *b = firstOne->GetBits();
	curSizeInBytes -= ((int *) b)[0];

	return 1;
}


int Page :: Append (Record *addMe) {
	char *b = addMe->GetBits();

	// first see if we can fit the record
	if (curSizeInBytes + ((int *) b)[0] > PAGE_SIZE) {
		return 0;
	}

	// move to the last record
	myRecs->MoveToFinish ();

	// and add it
	curSizeInBytes += ((int *) b)[0];
	myRecs->Insert(addMe);
	numRecs++;

	return 1;	
}


void Page :: ToBinary (char *bits) {

	// first write the number of records on the page
	((int *) bits)[0] = numRecs;

	char *curPos = bits + sizeof (int);

	// and copy the records one-by-one
	myRecs->MoveToStart ();
	for (int i = 0; i < numRecs; i++) {	
		char *b = myRecs->Current(0)->GetBits();
		
		// copy over the bits of the current record
		memcpy (curPos, b, ((int *) b)[0]);
		curPos += ((int *) b)[0];

		// and traverse the list
		myRecs->Advance ();
	}
}


void Page :: FromBinary (char *bits) {

	// first read the number of records on the page
	numRecs = ((int *) bits)[0];

	// sanity check
	if (numRecs > 1000000 || numRecs < 0) {
		cerr << "This is probably an error.  Found " << numRecs << " records on a page.\n";
		exit (1);
	}

	// and now get the binary representations of each
	char *curPos = bits + sizeof (int);

	// first, empty out the list of current records
	myRecs->MoveToStart ();
	while (myRecs->RightLength ()) {
		Record temp;
		myRecs->Remove(&temp);
	}

	// now loop through and re-populate it
	Record *temp = new (std::nothrow) Record();
	if (temp == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	curSizeInBytes = sizeof (int);
	for (int i = 0; i < numRecs; i++) {

		// get the length of the current record
		int len = ((int *) curPos)[0];
		curSizeInBytes += len;

		// create the record
		temp->CopyBits(curPos, len);

		// add it
		myRecs->Insert(temp);

		// and move along
		myRecs->Advance ();
		curPos += len;
	}

	delete temp;
}

File :: File () {
}

File :: ~File () {
}


void File :: GetPage (Page *putItHere, off_t whichPage) {

	// this is because the first page has no data
	whichPage++;

	if (whichPage >= curLength) {
		cerr << "whichPage " << whichPage << " length " << curLength << endl;
		cerr << "BAD: you tried to read past the end of the file\n";
		exit (1);
	}

	// read in the specified page
	char *bits = new (std::nothrow) char[PAGE_SIZE];
	if (bits == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	lseek (myFilDes, PAGE_SIZE * whichPage, SEEK_SET);
	read (myFilDes, bits, PAGE_SIZE);
	putItHere->FromBinary (bits);
	delete [] bits;
	
}


void File :: AddPage (Page *addMe, off_t whichPage) {

	// this is because the first page has no data
	whichPage++;

	// if we are trying to add past the end of the file, then
	// zero all of the pages out
	if (whichPage >= curLength) {
		
		// do the zeroing
		for (off_t i = curLength; i < whichPage; i++) {
			int foo = 0;
			lseek (myFilDes, PAGE_SIZE * i, SEEK_SET);
			write (myFilDes, &foo, sizeof (int));
		}

		// set the size
		curLength = whichPage + 1;	
	}

	// now write the page
	char *bits = new (std::nothrow) char[PAGE_SIZE];
	if (bits == NULL)
	{
		cout << "ERROR : Not enough memory. EXIT !!!\n";
		exit(1);
	}

	addMe->ToBinary (bits);
	lseek (myFilDes, PAGE_SIZE * whichPage, SEEK_SET);
	write (myFilDes, bits, PAGE_SIZE);
	delete [] bits;
#ifdef F_DEBUG
	cerr << " File: curLength " << curLength << " whichPage " << whichPage << endl;
#endif
}


void File :: Open (int fileLen, char *fName) {

	// figure out the flags for the system open call
        int mode;
        if (fileLen == 0)
                mode = O_TRUNC | O_RDWR | O_CREAT;
        else
                mode = O_RDWR;

	// actually do the open
        myFilDes = open (fName, mode, S_IRUSR | S_IWUSR);

#ifdef verbose
	cout << "Opening file " << fName << " with "<< curLength << " pages.\n";
#endif

	// see if there was an error
	if (myFilDes < 0) {
		cerr << "BAD!  Open did not work for " << fName << "\n";
		exit (1);
	}

	// read in the buffer if needed
	if (fileLen != 0) {

		// read in the first few bits, which is the page size
		lseek (myFilDes, 0, SEEK_SET);
		read (myFilDes, &curLength, sizeof (off_t));

	} else {
		curLength = 0;
	}

}


off_t File :: GetLength () {
	return curLength;
}


int File :: Close () {

	// write out the current length in pages
	lseek (myFilDes, 0, SEEK_SET);
	write (myFilDes, &curLength, sizeof (off_t));

	// close the file
	close (myFilDes);

	// and return the size
	return curLength;
	
}


