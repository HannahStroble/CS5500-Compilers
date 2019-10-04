/***********************
Author: Hannah Reinbolt
Date: 9/9/2019
Assignment: Homework #2

This assignment was not able to be completed in time. 
The rules are still being written out. 
Some are complete, but not all. Thus it does not function. 

How to run manually:
>>> g++ reinbolth.cpp lexer.cpp
>>> ./a.out input.txt > output.out
>>> vim output.out
***********************/

#include <iostream>
#include "lexer.h"
#include <string>
#include <typeinfo>
#include <stdio.h>
#include <stdbool.h>
#include <fstream>
#include <sstream>
using namespace std;

// global
bool debugRule = true;
string currentToken = "";
string nextToken = "";

// print statements
void printRule(string start, string dest)
{
  if(debugRule == true)
  {
    cout << start << " -> " << dest << endl;
  }
}

void printErr(string rule, string item)
{
  cout << "Error: Rule " << rule << " expecting " << item << endl;
}


void n_proglbl(Lexer &lex)
{
  printRule("N_PROGLBL", "T_PROG");
  if(currentToken == "T_PROG")
  {
    currentToken = lex.getToken();
  }
  else
  {
    printErr("N_PROGLBL", "T_PROG");
    exit(1);
  }
  
}


void n_block(Lexer &lex)
{
  printRule("N_BLOCK", "N_VARDECPART N_PROCDECPART N_STMTPART");
  // send to vardecpart
  n_vardecpart(lex);
  
  //send to procdecpart
  n_procdecpart(lex);
  
  // send to stmtpart
  n_stmtpart(lex);
  
}
/*
void n_vardecpart(Lexer &lex)
{
  
  if(currentToken == "T_VAR")
  {
    printRule("N_VARDECPART", "T_VAR N_VARDEC T_SCOLON N_VARDECLST")
    
    // send to vardec
    n_vardec(lex);
    
    // check if current token is ident
    currentToken = lex.getToken();
    if (currentToken == "T_SCOLON")
    {
        currentToken = lex.getToken();
    }
    else
    {
        printErr("N_VARDECPART", "T_SCOLON");
        exit(1);
    }
  
    // send to vardeclst
    n_vardeclist(lex);
  }
  else
  {
    // epsilon
    printRule("N_VARDECPART", "epsilon");
  }
}

void n_vardeclst(Lexer &lex)
{
  if(currentToken == "T_SCOLON")
  {
	printRule("N_VARDECLST", "N_VARDEC T_SCOLON N_VARDECLST")
     // send to vardec
     n_vardec(lex);
     
     // check if current token is semicolon
    if (currentToken == "T_SCOLON")
    {
        currentToken = lex.getToken();
    }
    else
    {
      printErr("N_VARDECLST", "T_SCOLON");
      exit(1);
    }
    
    // send to vardeclst
    n_vardeclst(lex);
  }
  else
  {
    // epsilon
    printRule("N_VARDECLST", "epsilon");
  }
}

void n_vardec(Lexer &lex)
{
  printRule("N_VARDEC", "N_IDENT N_IDENTLST T_COLON N_TYPE");
  
  // send to ident
  n_ident(lex);
  
  // send to identlst
  n_identlst(lex);
  
  if (currentToken == "T_COLON");
  {
    currentToken = lex.getToken();
  }
  else
  {
    printErr("N_VARDEC", "T_COLON");
  }
  
  // send to type
  n_type(lex);
}

void n_ident(Lexer &lex)
{
  printRule("N_IDENT", "T_IDENT");
  if (currentToken == "T_IDENT")
  {
      currentToken = lex.getToken();
  }
  else
  {
      printErr("N_IDENT", "T_IDENT");
      exit(1);
  }
}

void n_identlst(Lexer &lex)
{
  if(currentToken == "T_COMMA")
  { 
    printRule("N_IDENTLST", "T_COMMA N_IDENT N_IDENTLST");
    currentToken = lex.getToken();
    
    // send to ident
    n_ident(lex);
    
    // send to identlst
    n_identlst(lex);    
  }
  else
  {
    // epsilon
    printRule("N_IDENTLST", "epsilon");
  }
}

void n_type(Lexer &lex)
{
  nextToken = lex.lookAhead();
  if(nextToken == "T_ARRAY")
  { 
    printRule("N_TYPE", "N_ARRAY");
    currentToken = lex.getToken();
    n_array(lex);    
  }
  else
  {
    printRule("N_TYPE", "N_SIMPLE");
    n_simple(lex);
  }
}

void n_array(Lexer &lex)
{
  printRule("N_ARRAY", "T_ARRAY T_LBRACKET N_IDXRANGE T_RBRACKET T_OF N_SIMPLE");
  
  // array
  if (currentToken == "T_ARRAY")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_ARRAY", "T_ARRAY");
      exit(1);
  }
  
  // [
  if (currentToken == "T_LBRACKET")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_ARRAY", "T_LBRACKET");
      exit(1);
  }
  
  // send to idxrange
  n_idxrange(lex);
  
  // ]
  if (currentToken == "T_RBRACKET")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_ARRAY", "T_RBRACKET");
      exit(1);
  }
  
  // of
  if (currentToken == "T_OF")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_ARRAY", "T_OF");
      exit(1);
  }
  
  // send to simple
  n_simple(lex);
}

void n_idx(Lexer &lex)
{
  printRule("N_IDX", "T_INTCONST");
  
  // intconst
  if (currentToken == "T_INTCONST")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_IDX", "T_INTCONST");
      exit(1);
  }
  
}

void n_idxrange(Lexer &lex)
{
  printRule("N_IDXRANGE", "N_IDX T_DOTDOT N_IDX");
  
  // send to idx
  n_idx(lex);
  
  // ..
  if (currentToken == "T_DOTDOT")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_IDXRANGE", "T_DOTDOT");
      exit(1);
  }
  
  // send to idx
  n_idx(lex);
}

void n_simple(Lexer &lex)
{
  // integer
  if (currentToken == "T_INT")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_IDXRANGE", "T_INT");
      exit(1);
  }
  
  // char
  if else (currentToken == "T_CHAR")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_IDXRANGE", "T_CHAR");
      exit(1);
  }
  // bool
  if else (currentToken == "T_BOOL")
  {
      currentToken = lex.getToken();
  }
  else 
  {
      printErr("N_IDXRANGE", "T_BOOL");
      exit(1);
  }
}

void n_procdecpart(Lexer &lex)
{
  nextToken = lex.lookAhead();
  if(nextToken == "T_SCOLON")
  {
    printRule("N_PROCDECPART", "N_PROCDEC T_SCOLON N_PROCDECPART"); 
    n_procdec(lex);
    currentToken = lex.getToken();
    
    // ;
    if (currentToken == "T_SCOLON")
    {
      currentToken = lex.getToken();
    }
    else 
    {
      printErr("N_PROCDECPART", "T_SCOLON");
      exit(1);
    }
  
    n_procdecpart(lex);
  }
  else
  {
    // epsilon  
    printRule("N_PROCDECPART", "epsilon");
  }
}

void n_procdec(Lexer &lex)
{
  printRule("N_PROCDEC", "N_PROCHDR N_BLOCK");
  n_prochdr(lex);
  n_block(lex);
}

void n_prochdr(Lexer &lex)
{
  printRule("N_PROCHDR", "T_PROC T_IDENT T_SCOLON");
  
  // ;
    if (currentToken == "T_PROC")
    {
      currentToken = lex.getToken();
    }
    else 
    {
      printErr("N_PROCHDR", "T_PROC");
      exit(1);
    }
    
    // ident
    if (currentToken == "T_IDENT")
    {
      currentToken = lex.getToken();
    }
    else 
    {
      printErr("N_PROCHDR", "T_IDENT");
      exit(1);
    }
    
    // ;
    if (currentToken == "T_SCOLON")
    {
      currentToken = lex.getToken();
    }
    else 
    {
      printErr("N_PROCHDR", "T_SCOLON");
      exit(1);
    }
}

void n_stmtpart(Lexer &lex)
{
  printRule("N_STMTPART", "N_COMPOUND");
  n_compound(lex);
}

void n_compound(Lexer &lex)
{
  printRule("N_COMPOUND", "T_BEGIN N_STMT N_STMTLST T_END");
  
  // begin
    if (currentToken == "T_BEGIN")
    {
      currentToken = lex.getToken();
    }
    else 
    {
      printErr("N_COMPOUND", "T_BEGIN");
      exit(1);
    }
  
  n_stmt(lex);
  n_stmtlst(lex);
  
  // end
    if (currentToken == "T_END")
    {
      currentToken = lex.getToken();
    }
    else 
    {
      printErr("N_COMPOUND", "T_END");
      exit(1);
    }
}


void n_stmtlst(Lexer &lex, vector<string> &currentToken)
{
  if(currentToken[0] == "T_SCOLON")
  {
    printRule("N_STMTLST", "T_SCOLON N_STMT N_STMTLST");
    // ;
    n_stmt(lex, currentToken);
    n_stmtlst(lex, currentToken);
  }
  else
  {
    // epsilon
    printRule("N_STMTLST", "epsilon");
  }
}

void n_stmt(Lexer &lex, vector<string> &currentToken)
{
  if(currentToken[0] == "T_ASSIGN")
  {
    printRule("N_STMT", "N_ASSIGN");
    n_assign(lex, currentToken);
  }
  if else(currentToken[0] == "T_READ")
  {
    printRule("N_STMT", "N_READ");
    n_read(lex, currentToken);
  }
  if else(currentToken[0] == "T_READ")
  {
    printRule("N_STMT", "N_WRITE");
    n_write(lex, currentToken);
  }
  if else(currentToken[0] == "T_WHILE")
  {
    printRule("N_STMT", "N_WHILE");
    n_while(lex, currentToken);
  }
  if else(currentToken[0] == "########## compound")
  {
    printRule("N_STMT", "N_COMPOUND");
    n_compound(lex, currentToken);
  }
  if else(currentToken[0] == "############# condition")
  {
    printRule("N_STMT", "N_CONDITION");
    n_condition(lex, currentToken);
  }
  else
  {
    // error
  }
}

void n_assign(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_ASSIGN", "N_VARIABLE T_ASSIGN N_EXPR");
  n_assign(lex, currentToken);
  // :=
  n_expr(lex, currentToken);
}

void n_procstmt(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_PROCSTMT", "N_PROCIDENT");
  n_procident(lex, currentToken);
}

void n_procident(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_PROCIDENT", "T_IDENT");
  // ident
}

void n_read(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_READ", "T_READ T_LPAREN N_INPUTVAR N_INPUTLST T_RPAREN");
  // read
  // (
  n_inputvar(lex, currentToken);
  n_inputlst(lex, currentToken);
  // )
}

void n_inputlst(Lexer &lex, vector<string> &currentToken)
{
  if (currentToken[0] == "T_COMMA")
  {
    printRule("N_INPUTLST", "T_COMMA N_INPUTVAR N_INPUTLST");
    // ,
    n_inputvar(lex, currentToken);
    n_inputlst(lex, currentToken);
  }
  else
  {
    // epsilon
    printRule("N_PROCSTMT", "epsilon");
  }
}

void n_inputvar(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_INPUTVAR", "N_VARIABLE");
  n_variable(lex, currentToken);
}

void n_write(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_WRITE", "T_WRITE T_LPAREN N_OUTPUT N_OUTPUTLST T_RPAREN");
  // write
  // (
  n_output(lex, currentToken);
  n_outputlst(lex, currentToken);
  // )
}

void n_outputlst(Lexer &lex, vector<string> &currentToken)
{
  if (currentToken[0] == "T_COMMA")
  {
    printRule("N_OUTPUTLST", "T_COMMA N_OUTPUT N_OUTPUTLST");
    // ,
    n_output(lex, currentToken);
    n_outputlst(lex, currentToken);
  }
  else
  {
    // epsilon
    printRule("N_OUTPUT", "epsilon");
  }
}

void n_output(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_OUTPUT", "N_EXPR");
  n_expr(lex, currentToken);
}

void n_condition(Lexer &lex, vector<string> &currentToken)
{
  printRule("N_CONDITION", "T_WRITE T_LPAREN N_OUTPUT N_OUTPUTLST T_RPAREN");
  if(currentToken == "")
  // write
  // (
  n_output(lex, currentToken);
  n_outputlst(lex, currentToken);
  // )
}

*/

void n_prog(Lexer &lex)
{
  printRule("N_PROG", "N_PROGLBL T_IDENT T_SCOLON N_BLOCK T_DOT");
  n_proglbl(lex);
  
  // check if current token is ident
  if (currentToken == "T_IDENT")
  {
      currentToken = lex.getToken();
  }
  else
  {
      printErr("N_PROG", "T_IDENT");
      exit(1);
  }
  
  // check if current token is semicolon
  if (currentToken == "T_SCOLON")
  {
      currentToken = lex.getToken();
  }
  else
  {
      printErr("N_PROG", "T_SCOLON");
      exit(1);
  }
  
  // send to block
  n_block(lex);
  
  // check if current token is dot
  if (currentToken == "T_DOT")
  {
      currentToken = lex.getToken();
  }
  else
  {
      printErr("N_PROG", "T_DOT");
      exit(1);
  }
}


int main(int argc, char* argv[])
{
// import file
cout << "File: " << argv[1] << endl;

// start main lex
Lexer lex;
vector<string> input;
string tmp;
ifstream fin;
string fname;
string firstToken;

// Get input filename from command line
  if(argc < 2) 
  {
    printf("Please enter a filename sweetie!\n");
    exit(1);
  }
  fname = string(argv[1]);

  fin.clear();  // reset connection
  fin.open(fname);
  if (!fin)
  {
    cout << "Unable to open amazing file " << fname << endl;
    exit(1);
  }

  // Read lines of input into vector
  while(getline(fin, tmp)) {
    input.push_back(tmp);
  }

// Tokenize input
lex.lexemes_to_tokens(input);
cout << "Finished compiling tokens."<<endl;

currentToken = lex.getToken();
//n_prog(lex);

fin.close();

return 0;
}




