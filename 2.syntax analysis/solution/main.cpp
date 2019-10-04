/*
  MIPL lexical analyzer
  Written by Dalton Cole (drcgy5@umsystem.edu) August 2019

  To compile: make
  To execute: driver input.txt
              where input.txt is some MIPL input file
*/

#include "lexer.h"
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char** argv) {
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

  fin.close();

  return 0;
}