/*
  MIPL Compiler Fall 2019

  Lexical analyzer written by Dalton Cole (drcgy5@mst.edu) 
  Everything else written by Jennifer Leopold (leopoldj@mst.edu)

  To compile: g++ -std=c++11 *.cpp -o driver.exe
              or (if you have the makefile)
              make
  To execute: driver.exe input.txt
              where input.txt is some MIPL input file
*/

#include <fstream>
#include <sstream>
#include <iostream>
#include "lexer.h"
#include "parser.h"
using namespace std;

int main(int argc, char** argv) 
{
  Lexer lex;
  vector<string> input;
  string tmp;
  ifstream fin;
  string fileName;

  // Get input filename from command line
  if(argc < 2) 
  {
    printf("You must specify a file in the command line!\n");
    exit(1);
  }
  fileName = string(argv[1]);

  fin.clear();  // reset connection
  fin.open(fileName);
  if (!fin)
  {
    cout << "Unable to open file " << fileName << endl;
    exit(1);
  }

  // Read lines of input into vector
  while(getline(fin, tmp)) {
    input.push_back(tmp);
  }

  // Tokenize input
  lex.lexemes_to_tokens(input);


  // Start parsing from the start symbol of the grammar,
  // giving it the 1st token
  vector<string> currentToken = lex.getToken();
  Parser parser;
  parser.prog(lex, currentToken);

  // At the end of parsing, currentToken should be the
  // end-of-file token (i.e., "")
  if (currentToken[0] != "")
    cout << "\nSyntax error: unexpected chars at end of program!\n";
  //else cout << "\n---- Completed parsing ----\n";

  //make this dump to a file when we're done
  cout << oal_prog.str();

  fin.close();

  return 0;
}