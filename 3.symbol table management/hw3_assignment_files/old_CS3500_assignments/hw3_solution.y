/* 
    minir.y

    flex minir.l
    bison minir.y
    g++ minir.tab.c -o parser
    ./parser < inputFileName
    
*/

%{
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <stack>
#include "SymbolTable.h"
using namespace std;

int line_num = 1;
bool assignment_statement = false;

stack<SYMBOL_TABLE> scopeStack; // stack of scope hashtables

void beginScope();
void endScope();
void cleanUp();
bool findEntryInAnyScope(const string the_name);

void printTokenInfo(const char* token_type, const char* lexeme);

void printRule(const char *, const char *);

int yyerror(const char *s) 
{
    printf("Line %d: %s\n", line_num, s);
    cleanUp();
    exit(1);
}

extern "C" 
{
    int yyparse(void);
    int yylex(void);
    int yywrap() { return 1; }
}

%}

%union {
    char* text;
};

%token T_IDENT T_INTCONST T_FLOATCONST T_UNKNOWN T_STRCONST 
%token T_IF T_ELSE
%token T_WHILE T_FUNCTION T_FOR T_IN T_NEXT T_BREAK 
%token T_TRUE T_FALSE T_QUIT
%token T_PRINT T_CAT T_READ T_LPAREN T_RPAREN T_LBRACE 
%token T_RBRACE T_LBRACKET
%token T_RBRACKET T_SEMICOLON T_COMMA T_ADD T_SUB 
%token T_MULT T_DIV T_MOD
%token T_POW T_LT T_LE T_GT T_GE T_EQ T_NE T_NOT T_AND 
%token T_OR T_ASSIGN T_LIST

%type <text> T_IDENT

/*
 *  To eliminate ambiguity in if/else
 */
%nonassoc   T_RPAREN 
%nonassoc   T_ELSE


%start N_START

%%

N_START         : N_EXPR
                {
                    printRule("START", "EXPR");
                    endScope();
                    printf("\n---- Completed parsing ----\n\n");
                    return 0;
                }
                ;
N_EXPR          : N_IF_EXPR
                {
                    printRule("EXPR", "IF_EXPR");
                }
                | N_WHILE_EXPR
                {
                    printRule("EXPR", "WHILE_EXPR");
                }
                | N_FOR_EXPR
                {
                    printRule("EXPR", "FOR_EXPR");
                }
                | N_COMPOUND_EXPR
                {
                    printRule("EXPR", "COMPOUND_EXPR");
                }
                | N_ARITHLOGIC_EXPR
                {
                    printRule("EXPR", "ARITHLOGIC_EXPR");
                }
                | N_ASSIGNMENT_EXPR
                {
                    printRule("EXPR", "ASSIGNMENT_EXPR");
                }
                | N_OUTPUT_EXPR
                {
                    printRule("EXPR", "OUTPUT_EXPR");
                }
                | N_INPUT_EXPR
                {
                    printRule("EXPR", "INPUT_EXPR");
                }
                | N_LIST_EXPR
                {
                    printRule("EXPR", "LIST_EXPR");
                }
                | N_FUNCTION_DEF
                {
                    printRule("EXPR", "FUNCTION_DEF");
                }
                | N_FUNCTION_CALL
                {
                    printRule("EXPR", "FUNCTION_CALL");
                }
                | N_QUIT_EXPR
                {
                    printRule("EXPR", "QUIT_EXPR");
                    exit(1);
                }
                ;

N_CONST         : T_INTCONST
                {
                    printRule("CONST", "INTCONST");
                }
                | T_STRCONST
                {
                    printRule("CONST", "STRCONST");
                }
                | T_FLOATCONST
                {
                    printRule("CONST", "FLOATCONST");
                }
                | T_TRUE
                {
                    printRule("CONST", "TRUE");
                }
                | T_FALSE
                {
                    printRule("CONST", "FALSE");
                }
                ;

N_ARITHLOGIC_EXPR : N_SIMPLE_ARITHLOGIC
                {
                    printRule("ARITHLOGIC_EXPR", 
                              "SIMPLE_ARITHLOGIC");
                }
                | N_SIMPLE_ARITHLOGIC N_REL_OP
                  N_SIMPLE_ARITHLOGIC
                {
                    printRule("ARITHLOGIC_EXPR", 
                              "SIMPLE_ARITHLOGIC REL_OP "
                              "SIMPLE_ARITHLOGIC");
                }
                ;

N_SIMPLE_ARITHLOGIC : N_TERM N_ADD_OP_LIST
                {
                    printRule("SIMPLE_ARITHLOGIC", 
                              "TERM ADD_OP_LIST");
                }
                ;

N_ADD_OP_LIST	: N_ADD_OP N_TERM N_ADD_OP_LIST
                {
                    printRule("ADD_OP_LIST", 
                              "ADD_OP TERM ADD_OP_LIST");
                }
                | /* epsilon */
                {
                    printRule("ADD_OP_LIST", "epsilon");
                }
                ;

N_TERM		: N_FACTOR N_MULT_OP_LIST
                {
                    printRule("TERM", 
                              "FACTOR MULT_OP_LIST");
                }
                ;

N_MULT_OP_LIST	: N_MULT_OP N_FACTOR N_MULT_OP_LIST
                {
                    printRule("MULT_OP_LIST", 
                              "MULT_OP FACTOR MULT_OP_LIST");
                }
                | /* epsilon */
                {
                    printRule("MULT_OP_LIST", "epsilon");
                }
                ;

N_FACTOR		: N_VAR
                {
                    printRule("FACTOR", "VAR");
                }
                | N_CONST
                {
                    printRule("FACTOR", "CONST");
                }
                | T_LPAREN N_EXPR T_RPAREN
                {
                    printRule("FACTOR", "( EXPR )");
                }
                | T_NOT N_FACTOR
                {
                    printRule("FACTOR", "! FACTOR");
                }
                ;

N_COMPOUND_EXPR : T_LBRACE N_EXPR N_EXPR_LIST T_RBRACE
                {
                    printRule("COMPOUND_EXPR",
                              "{ EXPR EXPR_LIST }");
                }
                ;

N_EXPR_LIST     : T_SEMICOLON N_EXPR N_EXPR_LIST
                {
                    printRule("EXPR_LIST", "; EXPR EXPR_LIST");
                }
                | /* epsilon */
                {
                    printRule("EXPR_LIST", "epsilon");
                }
                ;

N_IF_EXPR       : T_IF T_LPAREN N_EXPR T_RPAREN N_EXPR
                {
                    printRule("IF_EXPR", "IF ( EXPR ) EXPR");
                }
                | T_IF T_LPAREN N_EXPR T_RPAREN N_EXPR T_ELSE
                  N_EXPR
                {
                    printRule("IF_EXPR", 
                              "IF ( EXPR ) EXPR ELSE EXPR");
                }
                ;

N_WHILE_EXPR    : T_WHILE T_LPAREN N_EXPR T_RPAREN N_LOOP_EXPR
                {
                    printRule("WHILE_EXPR", 
                              "WHILE ( EXPR ) "
                              "LOOP_EXPR");
                }
                ;

N_FOR_EXPR      : T_FOR T_LPAREN T_IDENT
                {
                    printRule("FOR_EXPR", 
                              "FOR ( IDENT IN EXPR ) "
                              "LOOP_EXPR");
                    string lexeme = string($3);
                    if(!(scopeStack.top().findEntry(lexeme)))
                        printf("___Adding %s to symbol table\n",
                               $3);
                        bool success = scopeStack.top().addEntry(
                            SYMBOL_TABLE_ENTRY(lexeme,
                                               UNDEFINED));
                }
                T_IN N_EXPR T_RPAREN N_LOOP_EXPR
                ;

N_LOOP_EXPR     : N_EXPR
                {
                    printRule("LOOP_EXPR", "EXPR");
                }
                | N_BREAK_EXPR
                {
                    printRule("LOOP_EXPR", "BREAK_EXPR");
                }
                | N_NEXT_EXPR
                {
                    printRule("LOOP_EXPR", "NEXT_EXPR");
                }
                ;

N_BREAK_EXPR    : T_BREAK
                {
                    printRule("BREAK_EXPR", "BREAK");
                }
                ;

N_NEXT_EXPR     : T_NEXT
                {
                    printRule("NEXT_EXPR", "NEXT");
                }
                ;

N_LIST_EXPR     : T_LIST T_LPAREN N_CONST_LIST T_RPAREN
                {
                    printRule("LIST_EXPR", 
                              "LIST ( CONST_LIST )");
                }
                ;

N_CONST_LIST    : N_CONST T_COMMA N_CONST_LIST
                {
                    printRule("CONST_LIST", 
                              "CONST, CONST_LIST");
                }
                | N_CONST
                {
                    printRule("CONST_LIST", "CONST");
                }
                ;

N_ASSIGNMENT_EXPR : T_IDENT N_INDEX
                {
                    printRule("ASSIGNMENT_EXPR", 
                              "IDENT INDEX ASSIGN EXPR");
                    string lexeme = string($1);
                    if(!(scopeStack.top().findEntry(lexeme))) {
                        printf("___Adding %s to symbol table\n",
                               $1);
                        bool success = scopeStack.top().addEntry(
                            SYMBOL_TABLE_ENTRY(lexeme, UNDEFINED));
                    }
                }
                T_ASSIGN N_EXPR
                ;

N_INDEX :       T_LBRACKET T_LBRACKET N_EXPR T_RBRACKET
                T_RBRACKET
			    {
                    printRule("INDEX", " [[ EXPR ]]");
			    }
			    | /* epsilon */
                {
                    printRule("INDEX", " epsilon");
                }
                ;

N_QUIT_EXPR     : T_QUIT T_LPAREN T_RPAREN
                {
                    printRule("QUIT_EXPR", "QUIT()");
                }
                ;

N_OUTPUT_EXPR   : T_PRINT T_LPAREN N_EXPR T_RPAREN
                {
                    printRule("OUTPUT_EXPR", 
                              "PRINT ( EXPR )");
                }
                | T_CAT T_LPAREN N_EXPR T_RPAREN
                {
                    printRule("OUTPUT_EXPR", 
                              "CAT ( EXPR )");
                }
                ;

N_INPUT_EXPR    : T_READ T_LPAREN N_VAR T_RPAREN
                {
                    printRule("INPUT_EXPR", "READ ( VAR )");
                }
                ;

N_FUNCTION_DEF  : T_FUNCTION
                {
                    beginScope();
                }
                T_LPAREN N_PARAM_LIST T_RPAREN N_COMPOUND_EXPR
                {
                    printRule("FUNCTION_DEF",
                              "FUNCTION ( PARAM_LIST )"
                              " COMPOUND_EXPR");
                    endScope();
                }
                ;

N_PARAM_LIST    : N_PARAMS
                {
                    printRule("PARAM_LIST", "PARAMS");
                }
                | N_NO_PARAMS
                {
                    printRule("PARAM_LIST", "NO PARAMS");
                }
                ;

N_NO_PARAMS     : /* epsilon */
                {
                    printRule("NO_PARAMS", "epsilon");
                }
                ;

N_PARAMS        : T_IDENT
                {
                    printRule("PARAMS", "IDENT");
                    string lexeme = string($1);
                    printf("___Adding %s to symbol table\n", $1);
                    bool success = scopeStack.top().addEntry(
                        SYMBOL_TABLE_ENTRY(lexeme, UNDEFINED));
                    if(!success) {
                        yyerror("Multiply defined identifier");
                        return(0);
                    }
                }
                | T_IDENT T_COMMA N_PARAMS
                {
                    printRule("PARAMS", "IDENT, PARAMS");
                    string lexeme = string($1);
                    printf("___Adding %s to symbol table\n", $1);
                    bool success = scopeStack.top().addEntry(
                        SYMBOL_TABLE_ENTRY(lexeme, UNDEFINED));
                    if(!success) {
                        yyerror("Multiply defined identifier");
                        return(0);
                    }
                }
                ;

N_FUNCTION_CALL : T_IDENT T_LPAREN N_ARG_LIST T_RPAREN
                {
                    printRule("FUNCTION_CALL", "IDENT"
                              " ( ARG_LIST )");
                    if (!findEntryInAnyScope($1)) {
                        yyerror("Undefined identifier");
                        return(0);
                    }
                }
                ;

N_ARG_LIST      : N_ARGS
                {
                    printRule("ARG_LIST", "ARGS");
                }
                | N_NO_ARGS
                {
                    printRule("ARG_LIST", "NO_ARGS");
                }
                ;

N_NO_ARGS       : /* epsilon */
                {
                    printRule("NO_ARGS", "epsilon");
                }
                ;

N_ARGS          : N_EXPR
                {
                    printRule("ARGS", "EXPR");
                }
                | N_EXPR T_COMMA N_ARGS
                {
                    printRule("ARGS", "EXPR, ARGS");
                }
                ;

N_ADD_OP	     : T_ADD
                {
                    printRule("ADD_OP", "+");
                }
                | T_SUB
                {
                    printRule("ADD_OP", "-");
                }
                | T_OR
                {
                    printRule("ADD_OP", "|");
                }
                ;

N_MULT_OP      : T_MULT
                {
                    printRule("MULT_OP", "*");
                }
                | T_DIV
                {
                    printRule("MULT_OP", "/");
                }
                | T_AND
                {
                    printRule("MULT_OP", "&");
                }
                | T_MOD
                {
                    printRule("MULT_OP", "\%\%");
                }
                | T_POW
                {
                    printRule("MULT_OP", "^");
                }
                ;

N_REL_OP        : T_LT
                {
                    printRule("REL_OP", "<");
                }
                | T_GT
                {
                    printRule("REL_OP", ">");
                }
                | T_LE
                {
                    printRule("REL_OP", "<=");
                }
                | T_GE
                {
                    printRule("REL_OP", ">=");
                }
                | T_EQ
                {
                    printRule("REL_OP", "==");
                }
                | T_NE
                {
                    printRule("REL_OP", "!=");
                }
                ;

N_VAR           : N_ENTIRE_VAR
                {
                    printRule("VAR", "ENTIRE_VAR");
                }
                | N_SINGLE_ELEMENT
                {
                    printRule("VAR", "SINGLE_ELEMENT");
                }
                ;

N_SINGLE_ELEMENT : T_IDENT T_LBRACKET T_LBRACKET N_EXPR
                   T_RBRACKET T_RBRACKET
                {
                    printRule("SINGLE_ELEMENT", "IDENT"
                              " [[ EXPR ]]");
                    if (!findEntryInAnyScope($1)) {
                        yyerror("Undefined identifier");
                        return(0);
                    }
                }
                ;

N_ENTIRE_VAR    : T_IDENT
                {
                    printRule("ENTIRE_VAR", "IDENT");
                    if(!findEntryInAnyScope($1))
                    {
                        yyerror("Undefined identifier");
                        return(0);
                    }
                }
                ;

%%

#include "lex.yy.c"
extern FILE *yyin;

void printTokenInfo(const char* token_type, const char* lexeme)
{
    printf("TOKEN: %s \t\t LEXEME: %s\n", token_type, lexeme);
}

void printRule(const char *lhs, const char *rhs)
{
    printf("%s -> %s\n", lhs, rhs);
    return;
}

void beginScope() {
    scopeStack.push(SYMBOL_TABLE());
    printf("\n___Entering new scope...\n\n");
}

void endScope() {
    scopeStack.pop();
    printf("\n___Exiting scope...\n\n");
}

void cleanUp() {
    if (scopeStack.empty())
        return;
    else {
        scopeStack.pop();
        cleanUp();
    }
}

bool findEntryInAnyScope(const string the_name) {
    if (scopeStack.empty()) return(false);
    bool found = scopeStack.top().findEntry(the_name);
    if (found)
        return(true);
    else {
        SYMBOL_TABLE symbolTable = scopeStack.top();
        scopeStack.pop();
        found = findEntryInAnyScope(the_name);
        scopeStack.push(symbolTable); // restore stack to original state
        return(found);
    }
}

int main() 
{
    beginScope();
    do {
        yyparse();
    } while (!feof(yyin));

    cleanUp();

    return 0;
}