==============================================================================
032608-1719

readme 
=========
Previous drivers are not included!

This driver is for assignment 3 (relational operators)

To compile the driver, type
	make test.out

To run the driver, type 'test.out' followed by the query # you wish to execute
	./test.out [1-8]

The Queries:
==============
1. select * from partsupp where ps_supplycost < 500
2. select p_partkey, p_name, p_retailprice from part where p_retailprice < 100;
3. select sum (s_acctbal + (s_acctbal * .05)) from supplier;
4. select sum (ps_supplycost) from supplier, partsupp where s_suppkey = ps_suppkey;
5. select distinct ps_suppkey from partsupp where ps_supplycost < 100.11;
6. select sum (ps_supplycost) from supplier, partsupp where s_suppkey = ps_suppkey groupby s_nationkey;
------------------
7: select sum(ps_supplycost) from part, supplier, partsupp 
	where p_partkey = ps_partkey and s_suppkey = ps_suppkey and s_acctbal > 2500;
8: select l_orderkey, l_partkey, l_suppkey from lineitem
	where l_returnflag = 'R' and l_discount < 0.04 or 
	l_returnflag = 'R' and l_shipmode = 'MAIL';

Note: For queries 7 and 8, i have only provided a possible plan.. If you have time,  
	I encourage you to code up this plan (will be a good experience for your next assignment!)

Using the Driver:
==================

1. SETTINGS: 
	The test driver reads the locations of location variables (dbfile_dir, tpch_dir)
	from the file 'test.cat' 
		--The first line describes the catalog location => *catalog_path
		--The second line tells where to store created dbfiles  => *dbfile_dir
		--The third line is the location where tpch tables can be found => *tpch_dir

IMPORTANT: All additional files that you create (metadata and other things) 
should be stored in the location specified by dbfile_dir. Donot hard code 
specific paths into your files.Always use the dbfile_dir variable to 
determine the location to store additional files.

2. Next replace the stub files DBFile.h, DBFile.cc, BigQ.h, and BigQ.cc,
RelOp.h and RelOp.cc   with your own code. If you are using additional
headers, modify the Makefile to include them as well. Then compile and
execute the driver and follow the on-screen instructions.

Submission Instructions: 
========================= 

Same as a2 (Instead of creating directory a2, create directory a3. The turnin
file should be called a3.tar.gz)
The executable produced by your file should be called a3.out
-------------------------------------------------------------------------------
