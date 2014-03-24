/***********************************************************************
 * SECTION 1
 ***********************************************************************/
/* The code in %{ %} is included as it is in lex.yy.c file
 * it has C global variables, prototypes, and comments 
 */


%{

#include <string.h> // For strdup()
#include <stdlib.h> // For malloc()
#include "ParseTree.h"
#include "yyfunc.tab.h"

//int yyfunclineno = 1;
void yyfuncerror(char*s);

static YY_BUFFER_STATE yyfunc_buf_state;
void init_lexical_parser_func (char *src) { yyfunc_buf_state = yyfunc_scan_string (src); }
void close_lexical_parser_func () { yyfunc_delete_buffer (yyfunc_buf_state); }


%}


/******************************************************************************
 * SECTION 2
 ******************************************************************************/
/* This is the DEFINITION section which contains substitutions, code, and
 * start stats; will be copied into lex.yy.c
 */

/******************************************************************************
 * SECTION 3
 ******************************************************************************/
/* This is the RULES section which defines how to "scan" and what action
 * to take for each token
 */

%%

"("			return('(');

")"    	        	return(')');

"+"    	        	return('+');

"-"    	        	return('-');

"/"    	        	return('/');

"*"    	        	return('*');

-?[0-9]+ 	       {yyfunclval.actualChars = strdup(yyfunctext);
  			return(Int); 
		        }

-?[0-9]+\.[0-9]*       {yyfunclval.actualChars = strdup(yyfunctext); 
  			return(Float);
			}

[A-Za-z][A-Za-z0-9_-]* {yyfunclval.actualChars = strdup(yyfunctext);
  			return(Name);
			}     

\n                	yyfunclineno++;

[ \t]             	;

.                 	yyfuncerror("LEX_ERROR: invalid character");

%%

void yyfuncerror(char *s) {
  printf("%d: %s at %s\n", yyfunclineno, s, yyfunctext);
}

int yyfuncwrap(void){
  return 1;
}


