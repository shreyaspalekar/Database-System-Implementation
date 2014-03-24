#ifndef FUNCTION_H
#define FUNCTION_H
#include "Record.h"
#include "ParseTree.h"

#define MAX_DEPTH 100


enum ArithOp {PushInt, PushDouble, ToDouble, ToDouble2Down, 
	IntUnaryMinus, IntMinus, IntPlus, IntDivide, IntMultiply,
	DblUnaryMinus, DblMinus, DblPlus, DblDivide, DblMultiply};

struct Arithmatic {

	ArithOp myOp;
	int recInput;
	void *litInput;	
};

class Function {

private:

	Arithmatic *opList;
	int numOps;

	int returnsInt;

public:

	Function ();

	// this grows the specified function from a parse tree and converts
	// it into an accumulator-based computation over the attributes in
	// a record with the given schema; the record "literal" is produced
	// by the GrowFromParseTree method
	void GrowFromParseTree (struct FuncOperator *parseTree, Schema &mySchema);

	// helper function
	Type RecursivelyBuild (struct FuncOperator *parseTree, Schema &mySchema);

	// prints out the function to the screen
	void Print ();

	// applies the function to the given record and returns the result
	Type Apply (Record &toMe, int &intResult, double &doubleResult);
};
#endif
