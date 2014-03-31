
#ifndef HEAPFILE_H
#define HEAPFILE_H
#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"



class HeapFile : public GenericDBFile{

	char *file_path;
	Record* current;
	Page* readPage;
	Page* writePage;
	File* file;
	off_t pageIndex;
	off_t writeIndex;
	char* name;
	int writeIsDirty;
	int endOfFile;

public:
	HeapFile (); 
	~HeapFile();

	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	int Close ();

	void Load (Schema &myschema, char *loadpath);

	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);


};
#endif
