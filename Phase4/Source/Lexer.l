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
#include "y.tab.h"

int lineno = 1;
void yyerror(char*s);

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

"<"    		      	return('<');

">"    		 	return('>');

"=" 			return('=');

-?[0-9]+ 	       {yylval.actualChars = strdup(yytext);
  			return(Int); 
		        }

-?[0-9]+\.[0-9]*       {yylval.actualChars = strdup(yytext); 
  			return(Float);
			}

"OR"      		return(OR);

"AND"			return(AND);

\'[^'\n]*\' 	       {/* take care of ' in a string */
	 		if (yytext[yyleng - 2] == '\\') {
     				yymore();
   			} else {
				yylval.actualChars = strdup(yytext + 1);	
				yylval.actualChars[strlen(yylval.actualChars) - 1] = 0;
     				return(String);
   			}
			}

[A-Za-z][\.A-Za-z0-9_-]* {yylval.actualChars = strdup(yytext);
  			return(Name);
			}     

\n                	lineno++;

[ \t]             	;

.                 	yyerror("LEX_ERROR: invalid character");

%%

void yyerror(char *s) {
  printf("%d: %s at %s\n", lineno, s, yytext);
}

int yywrap(void){
  return 1;
}


