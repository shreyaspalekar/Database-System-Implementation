 
%{

	#include "ParseTree.h" 
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <iostream>

	extern "C" int yylex();
	extern "C" int yyparse();
	extern "C" void yyerror(char *s);
  
	// this is the final parse tree that is returned	
	struct AndList *final;	

%}

// this stores all of the types returned by production rules
%union {
 	struct Operand *myOperand;
	struct ComparisonOp *myComparison; 
  	struct OrList *myOrList;
  	struct AndList *myAndList;
	char *actualChars;
}


%token OR
%token AND

%token <actualChars> Name
%token <actualChars> String
%token <actualChars> Float
%token <actualChars> Int

%type <myOrList> OrList
%type <myAndList> AndList 
%type <myComparison> Condition 
%type <myOperand> Literal 
%type <myComparison> Op 

%start AndList 


//******************************************************************************
// SECTION 3
//******************************************************************************
/* This is the PRODUCTION RULES section which defines how to "understand" the 
 * input language and what action to take for each "statment"
 */

%%

AndList: '(' OrList ')' AND AndList
{
	// here we need to pre-pend the OrList to the AndList
	// first we allocate space for this node
	$$ = (struct AndList *) malloc (sizeof (struct AndList));
	final = $$;

	// hang the OrList off of the left
	$$->left = $2;

	// hang the AndList off of the right
	$$->rightAnd = $5;

}

| '(' OrList ')'
{
	// just return the OrList!
	$$ = (struct AndList *) malloc (sizeof (struct AndList));
	final = $$;
	$$->left = $2;
	$$->rightAnd = NULL;
}
;

OrList: Condition OR OrList
{ 
	// here we have to hang the condition off the left of the OrList
	$$ = (struct OrList *) malloc (sizeof (struct OrList));	
	$$->left = $1;
	$$->rightOr = $3;
}

| Condition
{
	// nothing to hang off of the right
	$$ = (struct OrList *) malloc (sizeof (struct OrList));
	$$->left = $1;
	$$->rightOr = NULL;
}
;

Condition: Literal Op Literal 
{
	// in this case we have a simple literal/variable comparison
	$$ = $2;
	$$->left = $1;
	$$->right = $3;
}
;

Op: '<' 
{
	// construct and send up the comparison
	$$ = (struct ComparisonOp *) malloc (sizeof (struct ComparisonOp));
	$$->code = LESS_THAN;
}  

| '>'
{
	// construct and send up the comparison
	$$ = (struct ComparisonOp *) malloc (sizeof (struct ComparisonOp));
	$$->code = GREATER_THAN;
}  

| '='
{
	// construct and send up the comparison
	$$ = (struct ComparisonOp *) malloc (sizeof (struct ComparisonOp));
	$$->code = EQUALS;
}  
;

Literal : String 
{
	// construct and send up the operand containing the string
	$$ = (struct Operand *) malloc (sizeof (struct Operand));
	$$->code = STRING;
	$$->value = $1;
} 

| Float
{
	// construct and send up the operand containing the FP number
	$$ = (struct Operand *) malloc (sizeof (struct Operand));
	$$->code = DOUBLE;
	$$->value = $1;
} 

| Int
{
	// construct and send up the operand containing the integer
	$$ = (struct Operand *) malloc (sizeof (struct Operand));
	$$->code = INT;
	$$->value = $1;
} 

| Name
{
	// construct and send up the operand containing the name 
	$$ = (struct Operand *) malloc (sizeof (struct Operand));
	$$->code = NAME;
	$$->value = $1;
}
;

%%

