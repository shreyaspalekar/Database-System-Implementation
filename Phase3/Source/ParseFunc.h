
#ifndef ParseFunc
#define ParseFunc

// these are the types of operands that can appear in a CNF expression
#ifndef NAME
#define DOUBLE 1
#endif

#ifndef NAME
#define INT 2
#endif

#ifndef NAME
#define NAME 4
#endif

struct FuncOperand {

	// this tells us the type of the operand: FLOAT, INT, STRING...
	int code;

	// this is the actual operand
	char *value;
};

struct FuncOperator {

	// this tells us which operator to use: '+', '-', ...
	int code;

	// these are the operators on the left and on the right
	struct FuncOperator *leftOperator;
	struct FuncOperand *leftOperand;
	struct FuncOperator *right;	

};

#endif
