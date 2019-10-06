////////////////////////////////////////////////////////////
/// @file	parser.h
/// @brief Parses a given input
////////////////////////////////////////////////////////////

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <iomanip>
#include <stack>
#include <list>
#include <string>
#include <stdlib.h>
#include "lexer.h"
#include "symbolTable.h"
using namespace std;

// Error messages (besides "Syntax error")
const string MULTIPLY_DEFINED = "Multiply defined identifier";
const string UNDEFINED_IDENT = "Undefined identifier";

class Parser 
{
  public:
	Parser() { };

	/// Output "syntax error" with line # of currentToken
	/// and exit
	void syntaxError(const vector<string> currentToken) 
       const; 
	/// Output msg with line # of currentToken and exit
	void printError(const vector<string> currentToken,
                     const string msg) const; 


	/// Output lhs and rhs of a production
	void printRule(const string lhs, const string rhs);


	/// Recursive-descent function for each
	/// non-terminal in the MIPL grammar.
	/// Pre: lex contains collection of tokens with
	/// iterator positioned at the most current token
	/// and currentToken contains the current token
	/// Post: lex iterator has been updated as function
	/// has asked for more tokens and currentToken now
	/// contains the next token to be processed

	void prog(Lexer &lex, vector<string> &currentToken);
	void progLbl(Lexer &lex, vector<string> &currentToken);
	void block(Lexer &lex, vector<string> &currentToken);
	void varDecPart(Lexer &lex, vector<string> &currentToken);
	void varDecLst(Lexer &lex, vector<string> &currentToken);
	void varDec(Lexer &lex, vector<string> &currentToken);
	void ident(Lexer &lex, vector<string> &currentToken);
	void identLst(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO identType(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO array(Lexer &lex, vector<string> &currentToken);
	int idx(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO idxRange(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO simple(Lexer &lex, vector<string> &currentToken);
	void procDecPart(Lexer &lex, vector<string> &currentToken);
	void procDec(Lexer &lex, vector<string> &currentToken);
	void procHdr(Lexer &lex, vector<string> &currentToken);
	void stmtPart(Lexer &lex, vector<string> &currentToken);
	void compoundStmt(Lexer &lex, vector<string> &currentToken);
	void stmtLst(Lexer &lex, vector<string> &currentToken);
	void stmt(Lexer &lex, vector<string> &currentToken);
	void assignStmt(Lexer &lex, vector<string> &currentToken);
	void procStmt(Lexer &lex, vector<string> &currentToken);
	void procIdent(Lexer &lex, vector<string> &currentToken);
	void readStmt(Lexer &lex, vector<string> &currentToken);
	void inputLst(Lexer &lex, vector<string> &currentToken);
	void inputVar(Lexer &lex, vector<string> &currentToken);
	void writeStmt(Lexer &lex, vector<string> &currentToken);
	void outputLst(Lexer &lex, vector<string> &currentToken);
	void output(Lexer &lex, vector<string> &currentToken);
	void conditionStmt(Lexer &lex, vector<string> &currentToken);
	void elsePart(Lexer &lex, vector<string> &currentToken);
	void whileStmt(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO expr(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO opExpr(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO simpleExpr(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO addOpLst(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO term(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO multOpLst(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO factor(Lexer &lex, vector<string> &currentToken);
	bool sign(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO addOp(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO multOp(Lexer &lex, vector<string> &currentToken);
	void relOp(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO variable(Lexer &lex, vector<string> &currentToken);
	bool idxVar(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO constant(Lexer &lex, vector<string> &currentToken);
	TYPE_INFO boolConst(Lexer &lex, vector<string> &currentToken);
	void AOP(Lexer &lex, vector<string> &currentToken);


	/// Symbol table mgt calls

	void prSymbolTableAddition(const string identName, 
                           const TYPE_INFO typeInfo);
	void beginScope();
	void endScope();
	void cleanUp();
	TYPE_INFO findEntryInAnyScope(const string theName);

	stack<SYMBOL_TABLE> scopeStack;    // stack of scope
                                        // hashtables
	list<string> variableNames;		  // list of declared 
                                        // variables





	static bool print_productions_flag; 
	///< Print productions while parsing

	static bool print_symbolTableMgt_flag; 
	///< Print symbol table mgt messages while parsing


};

#endif

