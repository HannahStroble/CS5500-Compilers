/*
     hw5.y

 	Bison specifications file for the HW #5 language.
 	CS 5500, Fall 2019.

      To create the executable:

        flex hw5.l
        bison hw5.y
        g++ hw5.tab.c -o hw5

      To execute:

        hw5 <inputFileName
        
    # Notes to Remember #
    Blocks:
    - START: Line after IF statement and line OF a LABLE
    - END: Line before the beginning of the next block. Will need to iterate twice.

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
#include <string>
using namespace std;

// Operator codes
#define OP_PLUS 101
#define OP_GT 102
#define OP_LT 103
#define OP_NE 104
#define OP_LE 105
#define OP_GE 106
#define OP_EQ 107
#define OP_ASSIGN_INDEXED_LHS 108
#define OP_ASSIGN_INDEXED_RHS 109
#define OP_STAR 110
#define OP_LABEL 111
#define OP_IF_FALSE 112
#define OP_GOTO 113
#define OP_NA -1

#define SIZE_OF_INT 4

#define DEBUG 0       // if 1, outputs tokens, productions,
                      // symbol table entries, etc.

int lineNum = 1; 
int tempNum = 0;
int labelNum = 0;

bool blockLookAhead = false;
bool keepOptimizing = true;

typedef char Cstring[5];

typedef struct 
{
  int opCode;
  Cstring arg1;
  Cstring arg2;
  Cstring result;
  Cstring arrayBaseName;
  int arraySubscriptForType;
} Instruction;

typedef struct
{
  int start;
  int end;
} block;

typedef vector<int> SubscriptInfo;
vector<block> blocks;
vector<int> deadCode;

map<char, SubscriptInfo> symbolTable;
SubscriptInfo currentSubscriptInfo;  

vector<Instruction> instrxList;

void addEntryToSymbolTable(const char id, 
                           const SubscriptInfo subscriptInfo);
SubscriptInfo findEntryInSymbolTable(const char id);
void outputSubscriptInfo(const SubscriptInfo v);
void addInstruction(const Instruction x);
void printInstruction(const Instruction x);
int generateNewTemp();
int generateNewLabel();
void printTypeInfo(const char ch, const SubscriptInfo s);
int computeArrayElementWidth(const SubscriptInfo s, 
                             const int startIndex);

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
  Instruction instrx;
  Cstring cstring;
};

/*
 *	Token declarations
*/
%token  T_LPAREN T_RPAREN T_LBRACK T_RBRACK
%token  T_SEMICOL T_PLUS T_ASSIGN
%token  T_GT T_LT T_NE T_GE T_LE T_EQ
%token  T_VAR T_LCURLY T_RCURLY
%token  T_IF T_THEN T_ELSE T_WHILE T_TRUE T_FALSE 
%token  T_INTCONST T_IDENT T_UNKNOWN

%type <num> T_INTCONST R
%type <ch> T_IDENT 
%type <instrx> E L B

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
	currentSubscriptInfo.insert(currentSubscriptInfo.begin(),
                                 $2);
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
	SubscriptInfo s = findEntryInSymbolTable($1);
	if (DEBUG) printTypeInfo($1, s);
	Instruction x;
	sprintf(x.result, "%c", $1);
	strcpy(x.arg1, $3.result);
	strcpy(x.arg2, "");
	x.opCode = OP_NA; 
	addInstruction(x);
	}
	| L T_ASSIGN E
	{
	prRule("A", "L = E");
	Instruction x;
	strcpy(x.result, $1.arrayBaseName);
	strcpy(x.arg1, $1.arg1);
	strcpy(x.arg2, $3.result);
	x.opCode = OP_ASSIGN_INDEXED_LHS; 
	addInstruction(x);
	}
	;
F	: T_IF T_LPAREN B T_RPAREN
	{
	Instruction x;
    	Cstring buffer;
	sprintf(buffer, "L%d", generateNewLabel());
	strcpy(x.result, buffer);   
	strcpy($<cstring>$, buffer);    // where to go if false
	strcpy(x.arg1, $3.result);
	strcpy(x.arg2, "");
	x.opCode = OP_IF_FALSE;
	addInstruction(x);
 	}
        T_THEN
	{
 	sprintf($<cstring>$, "L%d", 
             generateNewLabel());    // where to go when done
 	}
        S T_ELSE
	{
	Instruction x1, x2;
 	strcpy(x1.result, $<cstring>7); // L.done
	strcpy(x1.arg1, "");
	strcpy(x1.arg2, "");
	x1.opCode = OP_GOTO;
	addInstruction(x1);
	strcpy(x2.result, $<cstring>5);
	strcpy(x2.arg1, "");
	strcpy(x2.arg2, "");
	x2.opCode = OP_LABEL;
	addInstruction(x2);
	}
        S
	{
	prRule("F", "if ( B ) then S else S");
	Instruction x;
	strcpy(x.result, $<cstring>7);  // L.done
	strcpy(x.arg1, "");
	strcpy(x.arg2, "");
	x.opCode = OP_LABEL;
	addInstruction(x);
	}
	; 
W	: T_WHILE T_LPAREN 
	{
	Instruction x;
	Cstring buffer;
	sprintf(buffer, "L%d", generateNewLabel());
	strcpy($<cstring>$, buffer);
	strcpy(x.result, buffer);
	strcpy(x.arg1, "");
	strcpy(x.arg2, "");
	x.opCode = OP_LABEL;
	addInstruction(x);
	}
       B 
     {
	Instruction x;
	Cstring buffer;
	sprintf(buffer, "L%d", generateNewLabel());
	strcpy(x.result, buffer);
	strcpy($<cstring>$, buffer);
	strcpy(x.arg1, $4.result);
	strcpy(x.arg2, "");
	x.opCode = OP_IF_FALSE;
     	addInstruction(x);
	}
       T_RPAREN S
	{
	prRule("S", "while ( B ) S");
	Instruction x1, x2;
  	strcpy(x1.result, $<cstring>3);
  	strcpy(x1.arg1, "");
  	strcpy(x1.arg2, "");
  	x1.opCode = OP_GOTO;
  	addInstruction(x1);
  	strcpy(x2.result, $<cstring>5);
  	strcpy(x2.arg1, "");
  	strcpy(x2.arg2, "");
  	x2.opCode = OP_LABEL;
  	addInstruction(x2);
	}
	;
E	: E T_PLUS T_INTCONST
	{
	prRule("E", "E + INTCONST");
  	Instruction x;
  	Cstring buffer;
  	sprintf(buffer, "t%d", generateNewTemp());
  	strcpy($$.result, buffer);
  	strcpy(x.result, buffer);
  	strcpy($$.arg1, $1.result);
  	strcpy(x.arg1, $1.result);
  	sprintf(buffer, "%d", $3);
  	strcpy($$.arg2, buffer);
  	strcpy(x.arg2, buffer);
  	$$.opCode = OP_PLUS; 
  	x.opCode = OP_PLUS; 
  	addInstruction(x);
	}
	| T_IDENT
	{
	prRule("E", "id");
	SubscriptInfo s = findEntryInSymbolTable($1);
	if (DEBUG) printTypeInfo($1, s);
    	sprintf($$.result, "%c", $1);
    	strcpy($$.arg1, "");
    	strcpy($$.arg2, "");
    	$$.opCode = OP_NA;
	}
	| L
	{
	prRule("E", "L");
    	Instruction x;
    	Cstring buffer;
    	sprintf(buffer, "t%d", generateNewTemp());
  	strcpy($$.result, buffer);
    	strcpy(x.result, buffer);
    	strcpy($$.arg1, $1.arrayBaseName);
    	strcpy(x.arg1, $1.arrayBaseName);
    	strcpy($$.arg2, $1.arg2); 
    	strcpy(x.arg2, $1.arg2);
    	$$.opCode = OP_ASSIGN_INDEXED_RHS;
    	x.opCode = OP_ASSIGN_INDEXED_RHS;
    	addInstruction(x);
	}
	| T_INTCONST
	{
	prRule("E", "INTCONST");
  	sprintf($$.result, "%d", $1);
  	strcpy($$.arg1, "");
  	strcpy($$.arg2, "");
  	$$.opCode = OP_NA;
	}
	;
L	: T_IDENT T_LBRACK E T_RBRACK
  	{
	prRule("L", "id [ E ]");
	SubscriptInfo s = findEntryInSymbolTable($1);
	if (DEBUG) printTypeInfo($1, s);
    	Cstring buffer;
    	Instruction x;
   	sprintf(buffer, "%c", $1);
    	strcpy($$.arrayBaseName, buffer);
  	$$.arraySubscriptForType = 1;
  	$$.opCode = OP_NA;
      
  	sprintf(buffer, "t%d", generateNewTemp());
  	strcpy(x.result, buffer);
  	strcpy($$.arg1, buffer);
  	strcpy($$.arg2, buffer);
  	strcpy($$.result, buffer);
  	strcpy(x.arg1, $3.result);
  	sprintf(buffer, "%d", computeArrayElementWidth(s, 1));
  	strcpy(x.arg2, buffer);
  	x.opCode = OP_STAR;
  	addInstruction(x);
	}
	| L T_LBRACK E T_RBRACK
	{
	prRule("L", "L [ E ]");
  	Instruction x1, x2;
    	Cstring buffer;
     	sprintf(buffer, "t%d", generateNewTemp());
     	strcpy(x1.result, buffer);
     	strcpy(x2.arg2, buffer);
     	sprintf(buffer, "t%d", generateNewTemp());
     	strcpy(x2.result, buffer);
     	strcpy($$.result, buffer);
     	strcpy($$.arg1, buffer);
     	strcpy(x2.arg1, $1.result);
     	x2.opCode = OP_PLUS;
     	strcpy(x1.arg1, $3.result);
     	x1.opCode = OP_STAR;
     	strcpy($$.arg2, buffer);
     	$$.opCode = OP_NA;
     	strcpy($$.arrayBaseName, $1.arrayBaseName);
     	SubscriptInfo s =
       findEntryInSymbolTable($1.arrayBaseName[0]);
	if (DEBUG) printTypeInfo($1.arrayBaseName[0], s);
     	$$.arraySubscriptForType = $1.arraySubscriptForType + 1;
     	sprintf(buffer, "%d", 
             computeArrayElementWidth(s,
					          $$.arraySubscriptForType));
     	strcpy(x1.arg2, buffer);
     	addInstruction(x1);
     	addInstruction(x2);
	}
	;
B	: E R E
     	{
	prRule("B", "E R E");
	Instruction x;
     	Cstring buffer;
     	sprintf(buffer, "t%d", generateNewTemp());
    	strcpy($$.result, buffer);
     	strcpy(x.result, buffer);
     	strcpy(x.arg1, $1.result);
    	strcpy(x.arg2, $3.result);
    	x.opCode = $2; 
    	addInstruction(x);
	}
	| T_TRUE
   	{
	prRule("B", "true");
    	strcpy($$.result, "true");
	}
	| T_FALSE
   	{
	prRule("B", "false");
   	strcpy($$.result, "false");
	}
	;                              
R	: T_GT
	{
	prRule("R", ">");
   	$$ = OP_GT;
	}
     	| T_LT
	{
	prRule("R", "<");
    	$$ = OP_LT;
	}
     	| T_NE
	{
	prRule("R", "!=");
     	$$ = OP_NE;
	}
	| T_GE
	{
	prRule("R", ">=");
    	$$ = OP_GE;
	}
    	| T_LE
	{
	prRule("R", "<=");
    	$$ = OP_LE;
	}
     	| T_EQ
	{
	prRule("R", "==");
     	$$ = OP_EQ;
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
                           const SubscriptInfo subscriptInfo) 
{
  // Make sure there isn't already an entry with the same name
  map<char, SubscriptInfo>::iterator itr;
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
SubscriptInfo findEntryInSymbolTable(const char id) 
{
  map<char, SubscriptInfo>::iterator itr;
  if ((itr = symbolTable.find(id)) == symbolTable.end())
    yyerror("Undefined identifier!");
  else return(itr->second);
}

// Output the contents of a vector of int's.
void outputSubscriptInfo(const SubscriptInfo s) 
{  
  for (int i = 0; i < s.size(); i++)
    printf("     s[%d] = %d\n", i, s[i]);
  return;
}

// Output the given 3-address Instruction to the instrxList.
void addInstruction(Instruction x) 
{
  instrxList.insert(instrxList.end(), x);
  if (DEBUG)
  {
    printf("\nJust added the following Instruction: ");
    printInstruction(x);
    printf("\n");
  }
  return;
}

// Output the given 3-address Instruction.
void printInstruction(const Instruction x) 
{
  char op[5];

  // Assign to 'op' a string representation of the
  // Instruction's opCode if it is arithmetic, relop, or
  // assignment
  switch (x.opCode) 
  {
    case OP_PLUS   : strcpy(op, "+"); break;
    case OP_GT     : strcpy(op, ">"); break;
    case OP_LT     : strcpy(op, "<"); break;
    case OP_NE     : strcpy(op, "!="); break;
    case OP_GE     : strcpy(op, ">="); break;
    case OP_LE     : strcpy(op, "<="); break;
    case OP_EQ     : strcpy(op, "=="); break;
    case OP_STAR   : strcpy(op, "*"); break;
    default        : strcpy(op, ""); 
  }

  // Output 3-address code for the Instruction
  switch (x.opCode) 
  {
    case OP_GOTO     : printf("goto %s\n", x.result);
                       break;

    case OP_IF_FALSE : printf("if %s == false goto %s\n", 
                              x.arg1, x.result);
                       break;

    case OP_LABEL    : printf("%s:\n", x.result);
                       break;

    case OP_ASSIGN_INDEXED_RHS : 
                       printf("%s = %s[%s]\n", 
                              x.result, x.arg1, x.arg2);
                       break;

    case OP_ASSIGN_INDEXED_LHS : 
                       printf("%s[%s] = %s\n", 
                              x.result, x.arg1, x.arg2);
                       break;

    default          : printf("%s = %s %s %s\n", x.result, 
                              x.arg1, op, x.arg2);
  }

  return;
}

// Return the current value of tempNum, and then increment it.
int generateNewTemp() 
{
  return(++tempNum);
}

// Return the current value of labelNum, and then increment it.
int generateNewLabel() 
{
  return(++labelNum);
}

// Output the identifier name (ch) and its subscript info.
void printTypeInfo(const char ch, const SubscriptInfo s) 
{
  printf("\n*** Found %c in symbol table\n", ch);
  if (s.size() > 0) {
    printf("*** This array has the following subscriptInfo:\n");
    outputSubscriptInfo(s);
  }
  printf("\n");
  return;
}

// Given a vector of subscript dimensions, compute the width
// as the product of the dimensions starting with dimension
// startIndex to the end, then multiply by SIZE_OF_INT.
int computeArrayElementWidth(const SubscriptInfo s, 
                             const int startIndex) 
{
  int w = 1;
  for (int i = startIndex; i < s.size(); i++)
    w = w * s[i];
  return(w * SIZE_OF_INT);
}

void printBlocks(vector<block> x)
{
  for (int i = 0; i < x.size(); i++)
  {
    cout << "BLOCK START: " << x[i].start << "\tBLOCK END: " << x[i].end << endl;
  }
}

int main( ) 
{
  // Parse until end-of-file
  do {
	yyparse();
  } while (!feof(yyin));
  
  
  //////// BLOCKS ////////
  // make initial block and blocks
  block theBlock;
  theBlock.start = 0;
  theBlock.end = -5;
  blocks.insert(blocks.end(), theBlock);
  

  // Make blocks
  for (int i = 0; i < instrxList.size(); i++)
  {
    // print 3 code
    //printInstruction(instrxList[i]);
    
    // check for label
    if (instrxList[i].opCode == OP_LABEL)
    {
      // do not add if duplicate there
      if ((blocks[i-1].start == i) || (blocks[0].start == i))
      {
        continue;
      }
      else
      {
      
        theBlock.start = i;
        theBlock.end = -5;
        blocks.insert(blocks.end(), theBlock);
      }
    }
    // check for IF statement
    else if ((instrxList[i].opCode == OP_IF_FALSE) || (blockLookAhead == true))
    {
      // do not add if duplicate there
      if ((blocks[i-1].start == i) || (blocks[0].start == i))
      {
        continue;
      }
      // only add for the line after the IF
      else if (blockLookAhead == true)
      {
        theBlock.start = i;
        theBlock.end = -5;
        blocks.insert(blocks.end(), theBlock);
        blockLookAhead = false;
      }
      else
      {
        blockLookAhead = true;
      }
    }
    
  }
  
  
  // Update the ends of the blocks
  for (int i = 0; i < blocks.size(); i++)
  {
    // skip the first entry for now
    if (i == 0)
    {
      continue;
    }
    // else update all the ends
    else
    {
      blocks[i-1].end = blocks[i].start - 1;
    }
    
  }
  // update last block
  blocks[blocks.size()-1].end = instrxList.size()-1;
  //printBlocks(blocks);
  
  ////////// PERFORM OPTIMIZATIONS //////////
  for (int i = 0; i < blocks.size(); i++)
  {
    //while(keepOptimizing)
   // {
      keepOptimizing = false;
      for (int j = blocks[i].start; j <= blocks[i].end; j++)
      {
  
  ////////// CONSTANT FOLDING ///////////////
  // eliminate temps, figure out what they were
    /*
      // search within arguments
      if (instrxList[j].arg1[0] == 't')
      {
        // find temps
        cout << "FOUND TEMP: " << instrxList[j].arg1 << endl;
      }
      
      // start out by printing all args and results
      cout << "result: ";
      printf(instrxList[j].result);
      cout << "   arg1: ";
      printf(instrxList[j].arg1);
      cout << "   arg2: ";
      printf(instrxList[j].arg2);
      cout << endl;
      */
      // put result in a dictionary
      // check both sides of the expression and see if there are variables
      // if variables then check dictionary for them
      // if they are not found then put a check to make sure another iteration happens
      // if they are there then replace with value found in dictionary
      
      keepOptimizing = false;
  
  ///////// ALGEBRAIC SIMPLIFICATION ///////
  // eliminate needless 0 or 1 addition or multiplication
  /*
      string t0 = to_string(0);
      char const *tmp0 = t0.c_str();
      string t1 = to_string(1);
      char const *tmp1 = t1.c_str();
  
      if ((instrxList[j].arg1 == tmp1)
              && (instrxList[j].opCode == OP_STAR) 
              && (instrxList[j].arg2.size() == 1))
      {
        instrxList[j].opCode = OP_NA;
        instrxList[j].arg1 = instrxList[j].arg2;
        instrxList[j].arg2 = "";
      }
      else if ((instrxList[j].arg2 == tmp1)
              && (instrxList[j].opCode == OP_STAR) 
              && (instrxList[j].arg1.size() == 1))
      {
        instrxList[j].opCode = OP_NA;
        instrxList[j].arg1 = instrxList[j].arg2;
        instrxList[j].arg2 = "";
      }
      
      if ((instrxList[j].arg1 == tmp0)
              && (instrxList[j].opCode == OP_PLUS) 
              && (instrxList[j].arg2.size() == 1))
      {
        instrxList[j].opCode = OP_NA;
        instrxList[j].arg1 = instrxList[j].arg2;
        instrxList[j].arg2 = "";
      }
      else if ((instrxList[j].arg2 == tmp0)
              && (instrxList[j].opCode == OP_PLUS) 
              && (instrxList[j].arg1.size() == 1))
      {
        instrxList[j].opCode = OP_NA;
        instrxList[j].arg1 = instrxList[j].arg2;
        instrxList[j].arg2 = "";
      }
      */
  
  
  ///////// COMMON SUBEXPRESSION ELIMINATION ///////
  
      }
   // }
  }
  
  //////// ELIMINATE DEAD CODE ////////////
  // things assigned to temps will be eliminated alltogether
  // deadCode vector stores all locations of dead code within instrxList
  // iterate through the list and delete dead code
  /*
  while(deadCode.empty() == false)
  {
    instrxList.erase(instrxList.begin() + deadCode.back());
    deadCode.pop_back();
  }
  */
  
  //////// OUTPUT 3-ADDRESS CODE PROGRAM //////////
  // print results
  for (int i = 0; i < instrxList.size(); i++)
  {
    cout << "(" << i << ") ";
    printInstruction(instrxList[i]);
  }
  

  return 0;
}
