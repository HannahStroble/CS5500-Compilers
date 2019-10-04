////////////////////////////////////////////////////////////
/// @file	parser.h
/// @brief Parses a given input
////////////////////////////////////////////////////////////

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include "lexer.h"
#include <iomanip>

using namespace std;


class Parser {
	public:
	Parser() { };

	/// Output "syntax error" with line # of currentToken
	void syntaxError(const vector<string> currentToken) 
     const; 

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
	void identType(Lexer &lex, vector<string> &currentToken);
	void array(Lexer &lex, vector<string> &currentToken);
	void idx(Lexer &lex, vector<string> &currentToken);
	void idxRange(Lexer &lex, vector<string> &currentToken);
	void simple(Lexer &lex, vector<string> &currentToken);
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
	void conditionStmt(Lexer &lex, 
                        vector<string> &currentToken);
	void elsePart(Lexer &lex, vector<string> &currentToken);
	void whileStmt(Lexer &lex, vector<string> &currentToken);
	void expr(Lexer &lex, vector<string> &currentToken);
	void opExpr(Lexer &lex, vector<string> &currentToken);
	void simpleExpr(Lexer &lex, vector<string> &currentToken);
	void addOpLst(Lexer &lex, vector<string> &currentToken);
	void term(Lexer &lex, vector<string> &currentToken);
	void multOpLst(Lexer &lex, vector<string> &currentToken);
	void factor(Lexer &lex, vector<string> &currentToken);
	void sign(Lexer &lex, vector<string> &currentToken);
	void addOp(Lexer &lex, vector<string> &currentToken);
	void multOp(Lexer &lex, vector<string> &currentToken);
	void relOp(Lexer &lex, vector<string> &currentToken);
	void variable(Lexer &lex, vector<string> &currentToken);
	void idxVar(Lexer &lex, vector<string> &currentToken);
	void constant(Lexer &lex, vector<string> &currentToken);
	void boolConst(Lexer &lex, vector<string> &currentToken);


	static bool print_productions_flag; 
	///< Print productions while parsing

};

#endif

