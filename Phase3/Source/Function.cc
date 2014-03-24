
#include "Function.h"
#include <iostream>
#include <stdlib.h>
Function :: Function () {

	opList = new Arithmatic[MAX_DEPTH];
}

Type Function :: RecursivelyBuild (struct FuncOperator *parseTree, Schema &mySchema) {

	// different cases; in the first case, simple, unary operation
	if (parseTree->right == 0 && parseTree->leftOperand == 0 && parseTree->code == '-') {

		// figure out the operations on the subtree
		Type myType = RecursivelyBuild (parseTree->leftOperator, mySchema);

		// and do the operation
		if (myType == Int) {
			opList[numOps].myOp = IntUnaryMinus;
			numOps++;
			return Int;

		} else if (myType == Double) {
			opList[numOps].myOp = DblUnaryMinus;
			numOps++;
			return Double;

		} else {
			cerr << "Weird type.\n";
			exit (1);
		}

	// in this case, we have either a literal value or a variable value, so do a push
	} else if (parseTree->leftOperator == 0 && parseTree->right == 0) {
		
		// now, there are two sub-cases.  In the first case, the value is from the
		// record that we are operating over, so we will find it in the schema
		if (parseTree->leftOperand->code == NAME) {

			// first, make sure that the attribute is there
			int myNum = mySchema.Find (parseTree->leftOperand->value);
			if (myNum == -1) {
				cerr << "Error!  Attribute in arithmatic expression was not found.\n";
				exit (1);
			}

			// it is there, so get the type
			int myType = mySchema.FindType (parseTree->leftOperand->value);

			// see if it is a string
			if (myType == String) {
				cerr << "Error!  No arithmatic ops over strings are allowed.\n";
				exit (1);
			}

			// everything is OK, so encode the instructions for loading from the rec
			if (myType == Int) {

				opList[numOps].myOp = PushInt;
				opList[numOps].recInput = myNum;
				opList[numOps].litInput = 0;
				numOps++;	
				return Int;
				
			// got a double
			} else {

				opList[numOps].myOp = PushDouble;
				opList[numOps].recInput = myNum;
				opList[numOps].litInput = 0;
				numOps++;	
				return Double;
			}
				
		// in this case, we have a literal value
		} else if (parseTree->leftOperand->code == INT) {

				// we were given a literal integer value!
				opList[numOps].myOp = PushInt;
				opList[numOps].recInput = -1;
				opList[numOps].litInput = (void *) (new int);
				*((int *) opList[numOps].litInput) = atoi (parseTree->leftOperand->value);
				numOps++;	
				return Int;

		} else {

				opList[numOps].myOp = PushDouble;
				opList[numOps].recInput = -1;
				opList[numOps].litInput = (void *) (new double);
				*((double *) opList[numOps].litInput) = atof (parseTree->leftOperand->value);
				numOps++;	
				return Double;
		}

	// now, we have dealt with the case of a unary negative and with an actual value
	// from the record or from the literal... last is to deal with an aritmatic op
	}  else {

		// so first, we recursively handle the left; this should give us the left
		// side's value, sitting on top of the stack
		Type myTypeLeft = RecursivelyBuild (parseTree->leftOperator, mySchema);

		// now we recursively handle the right
		Type myTypeRight = RecursivelyBuild (parseTree->right, mySchema);

		// the two values to be operated over are sitting on the stack.  So next we
		// do the operation.  But there are potentially some typing issues.  If both
		// are integers, then we do an integer operation 
		if (myTypeLeft == Int && myTypeRight == Int) {
			
			// integer operation!  So no casting required

			if (parseTree->code == '+') {
				opList[numOps].myOp = IntPlus;
				numOps++;
				return Int;

			} else if (parseTree->code == '-') {
				opList[numOps].myOp = IntMinus;
				numOps++;
				return Int;

			} else if (parseTree->code == '*') {
				opList[numOps].myOp = IntMultiply;
				numOps++;
				return Int;

			} else if (parseTree->code == '/') {
				opList[numOps].myOp = IntDivide;
				numOps++;
				return Int;

			} else {
				cerr << "Weird type!!!\n";
				exit (1);
			}

		}

		// if we got here, then at least one of the two is a double, so
		// the integer must be cast as appropriate
		if (myTypeLeft == Int) {
		
			// the left operand is an ant and needs to be cast
			opList[numOps].myOp = ToDouble2Down;
			numOps++;	
		}	

		if (myTypeRight == Int) {

                        // the left operand is an ant and needs to be cast
                        opList[numOps].myOp = ToDouble;
                        numOps++;
                }

		// now, we know that the top two items on the stach are doubles,
		// so we go ahead and do the math
		if (parseTree->code == '+') {
			opList[numOps].myOp = DblPlus;
			numOps++;
			return Double;

		} else if (parseTree->code == '-') {
			opList[numOps].myOp = DblMinus;
			numOps++;
			return Double;

		} else if (parseTree->code == '*') {
			opList[numOps].myOp = DblMultiply;
			numOps++;
			return Double;

		} else if (parseTree->code == '/') {
			opList[numOps].myOp = DblDivide;
			numOps++;
			return Double;

		} else {
			cerr << "Weird type!!!\n";
			exit (1);
		}
	}
}

void Function :: GrowFromParseTree (struct FuncOperator *parseTree, Schema &mySchema) {

	// zero out the list of operrations
	numOps = 0;

	// now recursively build the list
	Type resType = RecursivelyBuild (parseTree, mySchema);

	// remember if we get back an interger or if we get a double
	if (resType == Int)
		returnsInt = 1;
	else
		returnsInt = 0;

}

void Function :: Print () {

}

Type Function :: Apply (Record &toMe, int &intResult, double &doubleResult) {

	// this is rather simple; we just loop through and apply all of the 
	// operations that are specified during the function

	// this is the stack that holds the intermediate results from the 
	// function
	double stack[MAX_DEPTH];
	double *lastPos = stack - 1;
	char *bits = toMe.bits;

	for (int i = 0; i < numOps; i++) {

		switch (opList[i].myOp) {

			case PushInt: 

				lastPos++;	

				// see if we need to get the int from the record
				if (opList[i].recInput >= 0) {
					int pointer = ((int *) toMe.bits)[opList[i].recInput + 1];	
					*((int *) lastPos) = *((int *) &(bits[pointer]));

				// or from the literal value
				} else {
					*((int *) lastPos) = *((int *) opList[i].litInput);
				}

				break;

			case PushDouble: 

				lastPos++;	

				// see if we need to get the int from the record
				if (opList[i].recInput >= 0) {
					int pointer = ((int *) toMe.bits)[opList[i].recInput + 1];	
					*((double *) lastPos) = *((double *) &(bits[pointer]));

				// or from the literal value
				} else {
					*((double *) lastPos) = *((double *) opList[i].litInput);
				}

				break;

			case ToDouble:

				*((double *) lastPos) = *((int *) lastPos);
				break;

			case ToDouble2Down:

				*((double *) (lastPos - 1)) = *((int *) (lastPos - 1));
				break;

			case IntUnaryMinus:

				*((int *) lastPos) = -(*((int *) lastPos));
				break;

			case DblUnaryMinus:

				*((double *) lastPos) = -(*((double *) lastPos));
				break;

			case IntMinus:

				*((int *) (lastPos - 1)) = *((int *) (lastPos - 1)) -
						*((int *) lastPos);
				lastPos--;
				break;

			case DblMinus:

				*((double *) (lastPos - 1)) = *((double *) (lastPos - 1)) -
						*((double *) lastPos);
				lastPos--;
				break;

			case IntPlus:

				*((int *) (lastPos - 1)) = *((int *) (lastPos - 1)) +
						*((int *) lastPos);
				lastPos--;
				break;

			case DblPlus:

				*((double *) (lastPos - 1)) = *((double *) (lastPos - 1)) +
						*((double *) lastPos);
				lastPos--;
				break;

			case IntDivide:

				*((int *) (lastPos - 1)) = *((int *) (lastPos - 1)) /
						*((int *) lastPos);
				lastPos--;
				break;

			case DblDivide:

				*((double *) (lastPos - 1)) = *((double *) (lastPos - 1)) /
						*((double *) lastPos);
				lastPos--;
				break;

			case IntMultiply:

				*((int *) (lastPos - 1)) = *((int *) (lastPos - 1)) *
						*((int *) lastPos);
				lastPos--;
				break;

			case DblMultiply:

				*((double *) (lastPos - 1)) = *((double *) (lastPos - 1)) *
						*((double *) lastPos);
				lastPos--;
				break;

			default:
				
				cerr << "Had a function operation I did not recognize!\n";
				exit (1);	
		}
				
	}

	// now, we are just about done.  First we have a sanity check to make sure
	// that exactly one value is on the stack!
	if (lastPos != stack) {

		cerr << "During function evaluation, we did not have exactly one value ";
		cerr << "left on the stack.  BAD!\n";
		exit (1);

	}

	// got here, so we are good to go; just return the final value
	if (returnsInt) {
		intResult = *((int *) lastPos);
		return Int;
	} else {
		doubleResult = *((double *) lastPos);
		return Double;
	}
	
}


