/*
      hw5_parser.y

 	Bison specification file for the HW #5 language.
 	CS 5500, Fall 2019.

      To create the executable:

        flex hw5_parser.l
        bison hw5_parser.y
        g++ hw5_parser.tab.c -o hw5

      To execute:

        hw5 <inputFileName

*/

/*
 *	Declaration section.
 */
%{
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <cstring>
using namespace std;


int lineNum = 1; 

bool DEBUG  = true;   // if true, outputs tokens, productions,
                      // symbol table entries, etc.


typedef vector<int> SUBSCRIPT_INFO;

map<char, SUBSCRIPT_INFO> symbolTable;
SUBSCRIPT_INFO currentSubscriptInfo;  

void addEntryToSymbolTable(const char id, const SUBSCRIPT_INFO subscriptInfo);
SUBSCRIPT_INFO findEntryInSymbolTable(const char id);
void outputSubscriptInfo(const SUBSCRIPT_INFO v);
void printTypeInfo(const char ch, const SUBSCRIPT_INFO s);

void prRule(const char*, const char*);
void printTokenInfo(const char* tokenType, const char* lexeme);

int yyerror(const char* s) 
{
  printf("Line %d: %s\n", lineNum, s);
  return(1);
}

extern "C" 
{
    int yyparse(void);
    int yylex(void);
    int yywrap() {return 1;}
}

%}

%union {
  char ch;
  int num;
};

/*
 *	Token declarations
*/
%token  T_LPAREN T_RPAREN T_LBRACK T_RBRACK
%token  T_SEMICOL T_PLUS T_ASSIGN
%token  T_GT T_LT T_NE T_GE T_LE T_EQ
%token  T_VAR T_LCURLY T_RCURLY
%token  T_IF T_THEN T_ELSE T_WHILE T_DO T_TRUE T_FALSE 
%token  T_INTCONST T_IDENT T_UNKNOWN

%type <num> T_INTCONST R
%type <ch> T_IDENT

/*
 *	Starting point.
 */
%start		P

/*
 *	Translation rules.
 */
%%
P	: T_VAR V T_LCURLY C T_RCURLY
	{
		prRule("P", "var V { C }");
		if (DEBUG) printf("\n---- Completed parsing ----\n\n");
	}
	;


V   : /* epsilon */
	{
		prRule("V", "epsilon");
	}
	| T_IDENT N T_SEMICOL
	{
		addEntryToSymbolTable($1, currentSubscriptInfo);
		currentSubscriptInfo.clear( );
	}
	V
	{
		prRule("V", "id N ; V");
	}
	;


N	: T_LBRACK T_INTCONST T_RBRACK N
	{
		prRule("N", "[ INTCONST ] N");
		currentSubscriptInfo.insert(currentSubscriptInfo.begin(), $2);
	}
	| /* epsilon */
	{
		prRule("N", "epsilon");
	}
	;


C	: S T_SEMICOL C
	{
		prRule("C", "S ; C");
	}
	| /* epsilon */
	{
		prRule("C", "epsilon");
	}
	;


S	: A
	{
		prRule("S", "A ;");
	}
	| F
	{
		prRule("S", "F");
	}
	| W
	{
		prRule("S", "W");
	}
	;


A	: T_IDENT T_ASSIGN E
	{
		prRule("A", "id = E");
		SUBSCRIPT_INFO s = findEntryInSymbolTable($1);
		if (DEBUG) 
		{
	  		printf("\n*** Found %c in symbol table\n", $1);
   	  		if (s.size() > 0) 
	  		{
 	  			printf("*** This array has the following ");
	  			printf("subscriptInfo:\n");
	  			outputSubscriptInfo(s);
  	  		}
 	  		printf("\n");
		}
	}
	| L T_ASSIGN E
	{
		prRule("A", "L = E");
	}
	;


F	: T_IF T_LPAREN B T_RPAREN T_THEN S T_ELSE S
	{
		prRule("F", "if ( B ) then S else S");
	}
	;


W	: T_WHILE T_LPAREN B T_RPAREN S
	{
		prRule("S", "while ( B ) S");
	}
	;


E	: E T_PLUS T_INTCONST
	{
		prRule("E", "E + INTCONST");
	}
	| T_IDENT
	{
		prRule("E", "id");
	}
	| L
	{
		prRule("E", "L");
	}
	| T_INTCONST
	{
		prRule("E", "INTCONST");
	}
	;

L	: T_IDENT T_LBRACK E T_RBRACK
  	{
		prRule("L", "id [ E ]");
		SUBSCRIPT_INFO s = findEntryInSymbolTable($1);
		if (DEBUG) 
		{
  		  printf("\n*** Found %c in symbol table\n", $1);
   		  if (s.size() > 0) 
		  {
			  printf("*** This array has the following ");
			  printf("subscriptInfo:\n");
			  outputSubscriptInfo(s);
 		  }
		  printf("\n");
  		}
	}
	| L T_LBRACK E T_RBRACK
	{
		prRule("L", "L [ E ]");
	}
	;


B	: E R E
    {
		prRule("B", "E R E");
	}
	| T_TRUE
   	{
		prRule("B", "true");
	}
	| T_FALSE
   	{
		prRule("B", "false");
	}
	;


R	: T_GT
	{
		prRule("R", ">");
	}
     	| T_LT
	{
		prRule("R", "<");
	}
     	| T_NE
	{
		prRule("R", "!=");
	}
	| T_GE
	{
		prRule("R", ">=");
	}
    	| T_LE
	{
		prRule("R", "<=");
	}
     	| T_EQ
	{
		prRule("R", "==");
	}
	;


%%

#include "lex.yy.c"
extern FILE *yyin;

void prRule(const char* lhs, const char* rhs) 
{
  if (DEBUG) printf("%s -> %s\n", lhs, rhs);
  return;
}

void printTokenInfo(const char* tokenType, const char* lexeme) 
{
  if (DEBUG)
    printf("TOKEN: %-15s  LEXEME: %s\n", tokenType, lexeme);
  return;
}

// Add symbol table entry x to the symbol table.
void addEntryToSymbolTable(const char id, 
                           const SUBSCRIPT_INFO subscriptInfo) 
{
  // Make sure there isn't already an entry with the same name
  map<char, SUBSCRIPT_INFO>::iterator itr;
  if ((itr = symbolTable.find(id)) == symbolTable.end()) 
    symbolTable.insert(make_pair(id, subscriptInfo));
  else yyerror("Multiply defined identifier!");

  if (DEBUG) 
  {
    printf("\n>>> Added %c to symbol table\n", id);
    if (subscriptInfo.size() > 0) {
      printf(">>> with the following subscriptInfo:\n");
      outputSubscriptInfo(subscriptInfo);
    }
  }
  return;
}

// Return the subscript info for symbol table entry id.
SUBSCRIPT_INFO findEntryInSymbolTable(const char id) 
{
  map<char, SUBSCRIPT_INFO>::iterator itr;
  if ((itr = symbolTable.find(id)) == symbolTable.end())
    yyerror("Undefined identifier!");
  else return(itr->second);
}

// Output the contents of a vector of int's.
void outputSubscriptInfo(const SUBSCRIPT_INFO s) 
{  
  for (int i = 0; i < s.size(); i++)
    printf("     s[%d] = %d\n", i, s[i]);
  return;
}

// Output the identifier name (ch) and its subscript info.
void printTypeInfo(const char ch, const SUBSCRIPT_INFO s) 
{
  printf("\n*** Found %c in symbol table\n", ch);
  if (s.size() > 0) {
    printf("*** This array has the following subscriptInfo:\n");
    outputSubscriptInfo(s);
  }
  printf("\n");
  return;
}

int main( ) 
{
  // Parse until end-of-file
  do {
	yyparse();
  } while (!feof(yyin));

  // Output list of 3-address instructions
  // YOU NEED TO DO THIS!!!

  return 0;
}
