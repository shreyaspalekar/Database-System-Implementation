#include <iostream>
#include "DBFile.h"
#include "test.h"

// make sure that the file path/dir information below is correct
char *dbfile_dir = ""; // dir where binary heap files should be stored
char *tpch_dir ="/cise/tmp/dbi_sp11/DATA/10M/"; // dir where dbgen tpch files (extension *.tbl) can be found
char *catalog_path = "catalog"; // full path of the catalog file

using namespace std;

relation *rel;

// load from a tpch file
void test1 () {

	DBFile dbfile;
	cout << " DBFile will be created at " << rel->path () << endl;
	dbfile.Create (rel->path(), heap, NULL);

	char tbl_path[100]; // construct path of the tpch flat text file
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, rel->name()); 
	cout << " tpch file will be loaded from " << tbl_path << endl;

	dbfile.Load (*(rel->schema ()), tbl_path);
	dbfile.Close ();
}

// sequential scan of a DBfile 
void test2 () {

	DBFile dbfile;
	dbfile.Open (rel->path());
	dbfile.MoveFirst ();

	Record temp;

	int counter = 0;
	while (dbfile.GetNext (temp) == 1) {
		counter += 1;
		temp.Print (rel->schema());
		if (counter % 10000 == 0) {
			cout << counter << "\n";
		}
	}
	cout << " scanned " << counter << " recs \n";
	dbfile.Close ();
}

// scan of a DBfile and apply a filter predicate
void test3 () {

	cout << " Filter with CNF for : " << rel->name() << "\n";

	CNF cnf; 
	Record literal;
	rel->get_cnf (cnf, literal);

	DBFile dbfile;
	dbfile.Open (rel->path());
	dbfile.MoveFirst ();

	Record temp;

	int counter = 0;
	while (dbfile.GetNext (temp, cnf, literal) == 1) {
		counter += 1;
		temp.Print (rel->schema());
		if (counter % 10000 == 0) {
			cout << counter << "\n";
		}
	}
	cout << " selected " << counter << " recs \n";
	dbfile.Close ();
}

int main () {

	setup (catalog_path, dbfile_dir, tpch_dir);

	void (*test) ();
	relation *rel_ptr[] = {n, r, c, p, ps, o, li};
	void (*test_ptr[]) () = {&test1, &test2, &test3};  

	int tindx = 0;
	while (tindx < 1 || tindx > 3) {
		cout << " select test: \n";
		cout << " \t 1. load file \n";
		cout << " \t 2. scan \n";
		cout << " \t 3. scan & filter \n \t ";
		cin >> tindx;
	}

	int findx = 0;
	while (findx < 1 || findx > 7) {
		cout << "\n select table: \n";
		cout << "\t 1. nation \n";
		cout << "\t 2. region \n";
		cout << "\t 3. customer \n";
		cout << "\t 4. part \n";
		cout << "\t 5. partsupp \n";
		cout << "\t 6. orders \n";
		cout << "\t 7. lineitem \n \t ";
		cin >> findx;
	}

	rel = rel_ptr [findx - 1];
	test = test_ptr [tindx - 1];

	test ();

	cleanup ();
}
