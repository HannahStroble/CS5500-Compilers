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
 *  Declaration section.
 */
%{
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
using namespace std;

#define OPCODE_PL 1
#define OPCODE_EQ 2
#define OPCODE_NE 3
#define OPCODE_LE 4
#define OPCODE_GE 5
#define OPCODE_LT 6
#define OPCODE_GT 7
#define OPCODE_ST 8
#define OPCODE_GO 9
#define OPCODE_IL 10
#define OPCODE_IR 11
#define OPCODE_LB 12
#define OPCODE_IF 13
#define OPCODE_AS 14

int lineNum = 1; 
int globLabelCt = 1;
int globTempCt = 1;
bool DEBUG  = true;    // if true, outputs tokens, productions,
                        // symbol table entries, etc.

typedef char cstr[5]; //need to typedef to make the union happy
                      //maximum of five character per op section [TRUE\0]
                      //will tweak as needed
typedef struct
{
  int code;       //operation
  cstr a1;        //arg 1
  cstr a2;        //arg 2 (optional)
  cstr dest;      //destination
  cstr arrayBase; //Points to array base
  int arrayType;
} instruction;


typedef vector<int> SUBSCRIPT_INFO;

map<char, SUBSCRIPT_INFO> symbolTable;
SUBSCRIPT_INFO currentSubscriptInfo;  

std::vector<string> opVector;
std::vector<string> opScratch;


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
  instruction ins;
  cstr str;
};

/*
 *  Token declarations
*/
%token  T_LPAREN T_RPAREN T_LBRACK T_RBRACK
%token  T_SEMICOL T_PLUS T_ASSIGN
%token  T_GT T_LT T_NE T_GE T_LE T_EQ
%token  T_VAR T_LCURLY T_RCURLY
%token  T_IF T_THEN T_ELSE T_WHILE T_DO T_TRUE T_FALSE 
%token  T_INTCONST T_IDENT T_UNKNOWN

%type <num> T_INTCONST R
%type <ch> T_IDENT
%type <ins> E L B

/*
 *  Starting point.
 */
%start      P

/*
 *  Translation rules.
 */
%%
P   : T_VAR V T_LCURLY C T_RCURLY
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


N   : T_LBRACK T_INTCONST T_RBRACK N
    {
        prRule("N", "[ INTCONST ] N");
        currentSubscriptInfo.insert(currentSubscriptInfo.begin(), $2);
    }
    | /* epsilon */
    {
        prRule("N", "epsilon");
    }
    ;


C   : S T_SEMICOL C
    {
        prRule("C", "S ; C");
    }
    | /* epsilon */
    {
        prRule("C", "epsilon");
    }
    ;


S   : A
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


A   : T_IDENT T_ASSIGN E
    {
        opScratch.push_back(" = ");
        opScratch.push_back( string (1,$1));
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
        //instruction tmp
        //tmp.code = OPCODE_AS
        //tmp.dest = IDENT.result (IDENT = $1?) 
        //tmp.a1 = E.result(E = $3?)
        //add tmp
    }
    | L T_ASSIGN E
    {   
        prRule("A", "L = E");
        //instruction tmp
        //tmp.code = OPCODE_IL
        //tmp.dest = L.arrayBase <- this is the x part of x[y]
        //tmp.a1 = L.a1  <- the y part of ^^^
        //tmp.a2 = $3.dest <- this would beter be called result or something?
        //add tmp
    }
    ;


F   : T_IF T_LPAREN B T_RPAREN
    {
      prRule("F", "if ( B ) then S else S");
      printf("B1\n");
    }
    T_THEN //break out to execute code between blocks
    {
      printf("B2\n");
    }
    S T_ELSE //break out to execute code between blocks 
    {
      printf("B3\n");
    }
    S //break out to execute code between blocks
    {
      printf("B4\n");
    }
    ;


W   : T_WHILE T_LPAREN B T_RPAREN S
    {
        prRule("S", "while ( B ) S");
    }
    ;


E   : E T_PLUS T_INTCONST
    {
        prRule("E", "E + INTCONST");
    }
    | T_IDENT
    {
        
        opScratch.push_back(string (1,$1));
        prRule("E", "id");
    }
    | L
    {
        prRule("E", "L");
    }
    | T_INTCONST
    {
        opScratch.push_back(to_string($1));
        prRule("E", "INTCONST");
    }
    ;

L   : T_IDENT T_LBRACK E T_RBRACK
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


B   : E R E
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


R   : T_GT
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

void printIns(instruction in)
{
  cstr tmpOp;
  if      (in.code == OPCODE_PL) {strcpy(tmpOp, "+"  );}
  else if (in.code == OPCODE_EQ) {strcpy(tmpOp, "==" );}
  else if (in.code == OPCODE_NE) {strcpy(tmpOp, "!=" );}
  else if (in.code == OPCODE_LE) {strcpy(tmpOp, "<=" );}
  else if (in.code == OPCODE_GE) {strcpy(tmpOp, ">=" );}
  else if (in.code == OPCODE_LT) {strcpy(tmpOp, "<"  );}
  else if (in.code == OPCODE_GT) {strcpy(tmpOp, ">"  );}
  else if (in.code == OPCODE_ST) {strcpy(tmpOp, "*"  );}
  else                           {strcpy(tmpOp, ""   );}

  //special cases
  if(in.code == OPCODE_GO) //GOTO
  {
    printf("goto %s\n", in.dest);
  }
  else if(in.code == OPCODE_IL) //Index on L
  {
    printf("%s[%s] = %s\n", in.dest, in.a1, in.a2);
  }
  else if(in.code == OPCODE_IR) //Index on R
  {
    printf("%s = %s[%s]\n", in.dest, in.a1, in.a2);
  }
  else if(in.code == OPCODE_LB) //Label
  {
    printf("%s:\n", in.dest);
  }
  else if(in.code == OPCODE_IF) //FJMP
  {
    printf("if %s == false goto %s\n", in.a1, in.a2);
  }
  else if(in.code == OPCODE_AS) //normal op
  {
    printf("%s = %s\n", in.dest, in.a1);
  }
  else //normal op
  {
    printf("%s = %s %s %s\n", in.dest, in.a1, tmpOp, in.a2);
  }
  return;
}

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
  instruction x;
  char tz[] = "z";
  char tx[] = "x";
  char ty[] = "y";
  strcpy(x.dest, tz);
  strcpy(x.a1, tx);
  strcpy(x.a2, ty);

  x.code = OPCODE_AS;
  printIns(x);
  x.code = OPCODE_PL;
  printIns(x);
  x.code = OPCODE_EQ;
  printIns(x);
  x.code = OPCODE_NE;
  printIns(x);
  x.code = OPCODE_LE;
  printIns(x);
  x.code = OPCODE_GE;
  printIns(x);
  x.code = OPCODE_LT;
  printIns(x);
  x.code = OPCODE_GT;
  printIns(x);
  x.code = OPCODE_ST;
  printIns(x);
  x.code = OPCODE_GO;
  printIns(x);
  x.code = OPCODE_IL;
  printIns(x);
  x.code = OPCODE_IR;
  printIns(x);
  x.code = OPCODE_LB;
  printIns(x);
  x.code = OPCODE_IF;
  printIns(x);

  do {
    yyparse();
  } while (!feof(yyin));

  // Output list of 3-address instructions
  // YOU NEED TO DO THIS!!!
  reverse(opScratch.begin(), opScratch.end());
  while(!opScratch.empty())
  {
    //Unroll for loop, always work in threes
    string t1, t2, t3;
    t1 = opScratch.back(); 
    opScratch.pop_back();
    t2 = opScratch.back(); 
    opScratch.pop_back();
    t3 = opScratch.back(); 
    opScratch.pop_back();
    cout << t3 << t2 << t1 << endl;
  }
  return 0;
}
