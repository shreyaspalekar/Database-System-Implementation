#ifndef FILE_H
#define FILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "Comparison.h"
#include "ComparisonEngine.h"

class Record;

using namespace std;

class Page {
private:
	TwoWayList <Record> *myRecs;
	
	int numRecs;
	int curSizeInBytes;

public:
	// constructor
	Page ();
	virtual ~Page ();

	// this takes a page and writes its binary representation to bits
	void ToBinary (char *bits);

	// this takes a binary representation of a page and gets the
	// records from it
	void FromBinary (char *bits);

	// the deletes the first record from a page and returns it; returns
	// a zero if there were no records on the page
	int GetFirst (Record *firstOne);

	// this appends the record to the end of a page.  The return value
	// is a one on success and a aero if there is no more space
	// note that the record is consumed so it will have no value after
	int Append (Record *addMe);

	// empty it out
	void EmptyItOut ();

};


class File {
private:

	int myFilDes;
	off_t curLength; 

public:

	File ();
	~File ();

	// returns the current length of the file, in pages
	off_t GetLength ();

	// opens the given file; the first parameter tells whether or not to
	// create the file.  If the parameter is zero, a new file is created
	// the file; if notNew is zero, then the file is created and any other
	// file located at that location is erased.  Otherwise, the file is
	// simply opened
	void Open (int length, char *fName);

	// allows someone to explicitly get a specified page from the file
	void GetPage (Page *putItHere, off_t whichPage);

	// allows someone to explicitly write a specified page to the file
	// if the write is past the end of the file, all of the new pages that
	// are before the page to be written are zeroed out
	void AddPage (Page *addMe, off_t whichPage);

	// closes the file and returns the file length (in number of pages)
	int Close ();

};



#endif
