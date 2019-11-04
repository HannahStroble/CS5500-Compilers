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

/******************************
**  _  _  ___ _____ ___ ___  **
** | \| |/ _ \_   _| __/ __| **
** | .` | (_) || | | _|\__ \ **
** |_|\_|\___/ |_| |___|___/ **
**                           **
** $$ is left side of rules  **
**    (it is also autotyped) **
**                           **
** $n is nth right side rule **
**                           **
** R | A {1} B {2} C {3}     **
**    will do A              **
**    then {1}               **
**    then B, etc            **
**                           **
** $<type>n is nth as type   **
**                           **
*******************************/

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

#define INT_WIDTH 4

int lineNum = 1; 
int globLabelCt = 1;
int globTempCt = 1;
bool DEBUG  = false;
bool INS_DEBUG = false;  // if true, outputs tokens, productions,
                        // symbol table entries, etc.

typedef char cstr[5]; //need to typedef to make the union happy
                      //maximum of five character per op section [TRUE\0]
                      //will tweak as needed
typedef struct
{
  int code;       //operation
  cstr a1;        //arg 1
  cstr a2;        //arg 2 (optional)
  cstr addr;      //destination
  cstr arrayBase; //Points to array base
  int arrayNumIdx;
} instruction;

vector<instruction> iVector;


typedef vector<int> SUBSCRIPT_INFO;

map<char, SUBSCRIPT_INFO> symbolTable;
SUBSCRIPT_INFO currentSubscriptInfo;  

int arrWidth(SUBSCRIPT_INFO sub, int start);
void printIns(instruction in);
void addIns(instruction in);
int genLabel();
int genTemp();


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
  cstr strUnion;
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
        prRule("A", "id = E");
        instruction ins;
        SUBSCRIPT_INFO subInfo= findEntryInSymbolTable($1);
        sprintf(ins.addr, "%c", $1); //save to ID
        strcpy(ins.a1, $3.addr); //set it to whatever E is
        ins.code = OPCODE_AS;
        addIns(ins);

        if (DEBUG) 
        {
            printf("\n*** Found %c in symbol table\n", $1);
            if (subInfo.size() > 0) 
            {
                printf("*** This array has the following ");
                printf("subscriptInfo:\n");
                outputSubscriptInfo(subInfo);
            }
            printf("\n");
        }
    }
    | L T_ASSIGN E
    {   
        prRule("A", "L = E");
        instruction ins;
        strcpy(ins.addr, $1.arrayBase);
        strcpy(ins.a1, $1.a1); //get whatever L end up being
        strcpy(ins.a2, $3.addr); //set L to result of E
        ins.code = OPCODE_IL;
        addIns(ins);
    }
    ;


F   : T_IF T_LPAREN B T_RPAREN
    {
      prRule("F", "if ( B ) then S else S");
      instruction ins;
      cstr tmp;
      sprintf(tmp, "L%d", genLabel());
      strcpy(ins.addr, tmp);
      strcpy($<strUnion>$, tmp); //gotta specify or bison gets angry
      strcpy(ins.a1, $3.addr); //get B results
      ins.code = OPCODE_IF;
      addIns(ins);
    }
    T_THEN //break out to execute code between blocks
    {
      sprintf($<strUnion>$, "L%d", genLabel()); //pass the label name back up the tree
    }
    S T_ELSE //break out to execute code between blocks 
    {
      instruction ins1, ins2;
      strcpy(ins1.addr, $<strUnion>7); //$7 is our final state of S
      ins1.code = OPCODE_GO;
      addIns(ins1);

      strcpy(ins2.addr, $<strUnion>5); //grab that label from T_THEM
      ins2.code = OPCODE_LB;
      addIns(ins2);
    }
    S //break out to execute code between blocks
    {
      instruction ins;
      strcpy(ins.addr, $<strUnion>7); //label at start of S
      ins.code = OPCODE_LB;
      addIns(ins);
    }
    ;


W   : T_WHILE T_LPAREN
    {
      prRule("S", "while ( B ) S");
      instruction ins;
      cstr tmp;
      sprintf(tmp, "L%d", genLabel());
      strcpy($<strUnion>$, tmp); //pass label back up the stack
      strcpy(ins.addr, tmp);
      ins.code = OPCODE_LB;
      addIns(ins);
    }
    B 
    {
      instruction ins;
      cstr tmp;
      sprintf(tmp, "L%d", genLabel());
      strcpy($<strUnion>$, tmp); //pass the label back up
      strcpy(ins.addr, tmp);
      strcpy(ins.a1, $4.addr); //whatever S engs up being
      ins.code = OPCODE_IF;
      addIns(ins);
    }
    T_RPAREN S
    {
      instruction ins1, ins2;
      
      strcpy(ins1.addr, $<strUnion>3); //get B label
      ins1.code = OPCODE_GO;
      addIns(ins1);

      strcpy(ins2.addr, $<strUnion>5); //get exit label
      ins2.code = OPCODE_LB;
      addIns(ins2);
    }
    ;


E   : E T_PLUS T_INTCONST
    {
        prRule("E", "E + INTCONST");
        instruction ins;
        cstr tmp;

        sprintf(tmp, "t%d", genTemp());
        
        strcpy($$.addr, tmp);
        strcpy($$.a1, $1.addr);

        strcpy(ins.addr, tmp);
        strcpy(ins.a1, $1.addr);

        sprintf(tmp, "%d", $3); //Get the const as a string;
        
        strcpy($$.a2, tmp);
        $$.code = OPCODE_PL;

        strcpy(ins.a2, tmp);
        ins.code = OPCODE_PL;

        addIns(ins);

    }
    | T_IDENT
    {
        prRule("E", "id");
        SUBSCRIPT_INFO subInfo;
        subInfo = findEntryInSymbolTable($1);
        sprintf($$.addr, "%c", $1); //Get the id name
        $$.code = OPCODE_AS;
    }
    | L
    {
        prRule("E", "L");
        instruction ins;
        cstr tmp;
        sprintf(tmp, "t%d", genTemp()); // make a temp
        strcpy($$.addr, tmp); //Tell anything using this to get the temp holding the result
        strcpy($$.a1, $1.arrayBase); //get the base name of array to set tmp
        strcpy($$.a2, $1.a2);
        strcpy(ins.addr, tmp);
        strcpy(ins.a1, $1.arrayBase); //get the base name of array to compute
        strcpy(ins.a2, $1.a2);
        $$.code = OPCODE_IR; //return with right hand side
        ins.code = OPCODE_IR; //op with left hand index
        addIns(ins);


    }
    | T_INTCONST
    {
        prRule("E", "INTCONST");
        sprintf($$.addr, "%d", $1); //get the const
        $$.code = OPCODE_AS;
    }
    ;

L   : T_IDENT T_LBRACK E T_RBRACK
    {
        prRule("L", "id [ E ]");
        instruction ins;
        SUBSCRIPT_INFO subInfo;
        cstr tmp;

        //id exists? get it!
        subInfo = findEntryInSymbolTable($1);
        //stash it's name, two steps because strcpy don't do formatting :(
        sprintf(tmp, "%c",$1);
        strcpy($$.arrayBase, tmp);
        $$.arrayNumIdx = 1; //this is the first idx after the id
        $$.code = OPCODE_AS;

        sprintf(tmp, "t%d", genTemp());
        strcpy(ins.addr, tmp); //save to temp
        
        strcpy($$.a1, tmp);
        strcpy($$.a2, tmp); //if anything tries to use this, just give it a tmp handle
        strcpy($$.addr, tmp);

        sprintf(tmp, "%d", arrWidth(subInfo, 1)); //figure out the array unwind
        strcpy(ins.a1, $3.addr);
        strcpy(ins.a2, tmp);
        ins.code = OPCODE_ST;

        addIns(ins);
        

        subInfo = findEntryInSymbolTable($1);
        if (DEBUG) 
        {
          printf("\n*** Found %c in symbol table\n", $1);
          if (subInfo.size() > 0) 
          {
              printf("*** This array has the following ");
              printf("subscriptInfo:\n");
              outputSubscriptInfo(subInfo);
          }
          printf("\n");
        }
    }
    | L T_LBRACK E T_RBRACK
    {
        prRule("L", "L [ E ]");
        instruction ins1, ins2;
        SUBSCRIPT_INFO subInfo;
        cstr tmp;

        sprintf(tmp, "t%d", genTemp());
        strcpy(ins1.addr, tmp);
        strcpy(ins2.a2, tmp); //whoops wrong argument

        sprintf(tmp, "t%d", genTemp());
        strcpy($$.addr, tmp);
        strcpy($$.a1, tmp);
        strcpy($$.a2, tmp);
        $$.code = OPCODE_AS;
        
        strcpy(ins2.addr, tmp);
        strcpy(ins2.a1, $1.addr);
        ins2.code = OPCODE_PL;
        
        strcpy(ins1.a1, $3.addr);
        ins1.code = OPCODE_ST;

        
        //L.type = L1.type
        strcpy($$.arrayBase, $1.arrayBase);
        subInfo = findEntryInSymbolTable($1.arrayBase[0]);
        $$.arrayNumIdx = $1.arrayNumIdx + 1;

        sprintf(tmp, "%d", arrWidth(subInfo,$$.arrayNumIdx));
        strcpy(ins1.a2, tmp);
        addIns(ins1);
        addIns(ins2);
    }
    ;


B   : E R E
    {
        prRule("B", "E R E");
        instruction ins;
        cstr tmp;
        //sprintf to format
        sprintf(tmp, "t%d", genTemp());
        //We want parent to get the temp we build next
        strcpy($$.addr, tmp); 
        //Build our instruction
        ins.code = $2; //Whatever R is
        strcpy(ins.addr, tmp); //Save to our new temp friend
        strcpy(ins.a1, $1.addr); //first E
        strcpy(ins.a2, $3.addr); //Second E
        //Add to the vector
        addIns(ins);
    }
    | T_TRUE
    {
        prRule("B", "true");
        strcpy($$.addr, "true");
    }
    | T_FALSE
    {
        prRule("B", "false");
        strcpy($$.addr, "false");
    }
    ;


R   : T_GT
    {
        prRule("R", ">");
        $$ = OPCODE_GT;
    }
        | T_LT
    {
        prRule("R", "<");
        $$ = OPCODE_LT;
    }
    | T_NE
    {
        prRule("R", "!=");
        $$ = OPCODE_NE;
    }
    | T_GE
    {
        prRule("R", ">=");
        $$ = OPCODE_GE;
    }
        | T_LE
    {
        prRule("R", "<=");
        $$ = OPCODE_LE;
    }
        | T_EQ
    {
        prRule("R", "==");
        $$ = OPCODE_EQ;
    }
    ;


%%

#include "lex.yy.c"
extern FILE *yyin;

int genTemp()
{
    return globTempCt++;
}

int genLabel()
{
    return globLabelCt++;
}

void addIns(instruction in)
{
    iVector.push_back(in);
    if(INS_DEBUG)
    {
        printf("Added: ");
        printIns(in);
        printf("\n");
    }
    return;
}

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
    printf("goto %s\n", in.addr);
  }
  else if(in.code == OPCODE_IL) //Index on L
  {
    printf("%s[%s] = %s\n", in.addr, in.a1, in.a2);
  }
  else if(in.code == OPCODE_IR) //Index on R
  {
    printf("%s = %s[%s]\n", in.addr, in.a1, in.a2);
  }
  else if(in.code == OPCODE_LB) //Label
  {
    printf("%s:\n", in.addr);
  }
  else if(in.code == OPCODE_IF) //FJMP
  {
    printf("if %s == false goto %s\n", in.a1, in.addr); //output addr, not arg
  }
  else if(in.code == OPCODE_AS) //normal op
  {
    printf("%s = %s\n", in.addr, in.a1);
  }
  else //normal op
  {
    printf("%s = %s %s %s\n", in.addr, in.a1, tmpOp, in.a2);
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

int arrWidth(SUBSCRIPT_INFO sub, int start)
{
    int width = 1;
    for(int i = start; i < sub.size(); i++)
    {
        width *= sub[i];
    }
    return (width * INT_WIDTH);
}

int main( ) 
{
  // Parse until end-of-file
 do {
    yyparse();
  } while (!feof(yyin));

  // Output list of 3-address instructions
  // YOU NEED TO DO THIS!!!
 for(auto &ins : iVector)
 {
    printIns(ins);
 }
  return 0;
}
