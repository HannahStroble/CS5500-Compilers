#include "parser.h"
#include "SymbolTable.h"

// Static Parser variable
bool Parser::print_productions_flag = false; 
bool Parser::print_additions_flag = true; 
bool Parser::suppressTokenOutput = false;

std::vector<string> ident_tmp;
array_parts array_temp;
int tmp_simple;
bool isArray;
bool blockINI = false;
// Push a new SYMBOL_TABLE onto scopeStack.
void Parser::beginScope()
{
  scopeStack.push(SYMBOL_TABLE());
    if(!suppressTokenOutput)
      printf("\n\n>>> Entering new scope...\n");
}

// Pop a SYMBOL_TABLE from scopeStack.
void Parser::endScope() 
{
    scopeStack.pop();
    if(!suppressTokenOutput)
        printf("\n<<< Exiting scope...\n");
}

// Pop all SYMBOL_TABLE's from scopeStack.
void Parser::cleanUp() 
{
    if (scopeStack.empty())
        return;
    else {
        scopeStack.pop();
        cleanUp();
    }
}

bool Parser::findEntryInScope(const string the_name)
{
    if (scopeStack.empty()) return(false);
    bool found = scopeStack.top().findEntry(the_name);
    return found;
    // if (found)
    //     return(true);
    // else {
    //     SYMBOL_TABLE symbolTable = scopeStack.top();
    //     scopeStack.pop();
    //     found = findEntryInAnyScope(the_name);
    //     scopeStack.push(symbolTable); // restore stack to original state
    //     return(found);
    // }
}

bool Parser::findEntryInAnyScope(const string the_name)
{
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

void Parser::printAddition(const string item, const int item_type)
{
  if (print_additions_flag)
  {
    cout << "\n+++ Adding " << item << " to symbol table with type ";
    if(item_type == T_INT)
      cout << "INTEGER";
    else if (item_type == T_CHAR)
      cout << "CHAR";
    else if (item_type == T_BOOL)
      cout << "BOOLEAN";
    else if (item_type == T_PROG)
      cout << "PROGRAM";
    else if (item_type == T_PROC)
      cout << "PROCEDURE";

  cout << endl;
  }
  return;
}

void Parser::printAddition(const string item, const array_parts parts)
{
  if (print_additions_flag)
  {
    cout << "\n+++ Adding " << item << " to symbol table with type ARRAY " << parts.start << " .. " << parts.end << " OF ";
    if(parts.type == T_INT)
      cout << "INTEGER";
    else if (parts.type == T_CHAR)
      cout << "CHAR";
    else if (parts.type == T_BOOL)
      cout << "BOOLEAN";

  cout << endl;
  }
  return;
}

void Parser::printAddition(const string item, const string item_type)
{
  if (print_additions_flag)
    cout << "\n +++ Adding " << item << " to symbol table with type " << item_type << "\n" << endl; 
  return;
}


void Parser::printRule(const string lhs, const string rhs)
{
  if (print_productions_flag)
    cout << lhs << " -> " << rhs << endl; 
  return;
}

void Parser::syntaxError(const vector<string> currentToken) const 
{
  cout << "Line " << currentToken[2]
       << ": syntax error\n";  
  exit(1);
}


void Parser::prog(Lexer &lex, vector<string> &currentToken) 
{
  beginScope();
  printRule("N_PROG", 
            "N_PROGLBL T_IDENT T_SCOLON N_BLOCK T_DOT"); 
  progLbl(lex, currentToken);
  if (currentToken[0] == "T_IDENT") 
  {
    scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(currentToken[1], T_PROG));
    printAddition(currentToken[1], T_PROG);

    currentToken = lex.getToken();
    if (currentToken[0] == "T_SCOLON")
    {
      currentToken = lex.getToken();
      block(lex, currentToken);
      if (currentToken[0] == "T_DOT")
        currentToken = lex.getToken();
      else syntaxError(currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::progLbl(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_PROGLBL", "T_PROG");
  if (currentToken[0] == "T_PROG")
    currentToken = lex.getToken();
  else syntaxError(currentToken);
}
 
void Parser::block(Lexer &lex, vector<string> &currentToken) 
{
  if(!blockINI)
  {
    blockINI = true;
  }  
  else
  {
    beginScope();
  }
  printRule("N_BLOCK", 
            "N_VARDECPART N_PROCDECPART N_STMTPART");
  varDecPart(lex, currentToken);
  procDecPart(lex, currentToken);
  stmtPart(lex, currentToken);
  endScope();
}
 
void Parser::varDecPart(Lexer &lex, vector<string> &currentToken) {
  if (currentToken[0] == "T_VAR") 
  {
    printRule("N_VARDECPART",
              "T_VAR N_VARDEC T_SCOLON N_VARDECLST");
    currentToken = lex.getToken();
    varDec(lex, currentToken);
    if (currentToken[0] == "T_SCOLON")
    {
      currentToken = lex.getToken();
      varDecLst(lex, currentToken);
    }
    else syntaxError(currentToken);
  }
  else printRule("N_VARDECPART", "epsilon");
}

void Parser::varDecLst(Lexer &lex, vector<string> &currentToken)
{
  if (currentToken[0] == "T_IDENT")
  {
    printRule("N_VARDECLST", "N_VARDEC T_SCOLON N_VARDECLST");
    varDec(lex, currentToken);
    if (currentToken[0] == "T_SCOLON")
    {
      currentToken = lex.getToken();
      varDecLst(lex, currentToken);
    }
    else syntaxError(currentToken);
  }
  else printRule("N_VARDECLST", "epsilon");
}

void Parser::varDec(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_VARDEC", "N_IDENT N_IDENTLST T_COLON N_TYPE");
  ident(lex, currentToken);
  identLst(lex, currentToken);
  if (currentToken[0] == "T_COLON")
  {
    currentToken = lex.getToken();
    //get types in here...
    identType(lex, currentToken);
    for(auto& i : ident_tmp)
    {
      bool inScope = findEntryInScope(i);
      if(isArray)
      {
        scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(i, array_temp));
        printAddition(i, array_temp);
      }
      else
      {
        scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(i, tmp_simple));
        printAddition(i, tmp_simple);
      }
      if(inScope)
      {
        //already defined, bail out here
        cout << "Line " << currentToken[2] <<": Multiply defined identifier   "<<endl;
        exit(1);
      }
    }
  }
  //clear the vector of stashed idents
  else syntaxError(currentToken);
  ident_tmp.clear();
}

void Parser::ident(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_IDENT", "T_IDENT");
  if (currentToken[0] == "T_IDENT")
  {
    ident_tmp.push_back(currentToken[1]);
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
}

void Parser::identLst(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_COMMA")
  {
    printRule("N_IDENTLST", "T_COMMA N_IDENT N_IDENTLST");
    currentToken = lex.getToken();
    ident(lex, currentToken);
    identLst(lex, currentToken);
  }
  else printRule("N_IDENTLST", "epsilon");
}

void Parser::identType(Lexer &lex, vector<string> &currentToken)
{
  if (currentToken[0] == "T_ARRAY")
  {
    printRule("N_TYPE", "N_ARRAY");
    //array time!
    array(lex, currentToken);
    isArray = true;
  }
  else 
  {
    printRule("N_TYPE", "N_SIMPLE");
    //simple type
    tmp_simple = simple(lex, currentToken);
    isArray = false;
  }
}

void Parser::array(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_ARRAY",
            "T_ARRAY T_LBRACK N_IDXRANGE T_RBRACK T_OF"
            " N_SIMPLE");
  if (currentToken[0] == "T_ARRAY")
  {
    currentToken = lex.getToken();
    if (currentToken[0] == "T_LBRACK")
    {
      currentToken = lex.getToken();
      idxRange(lex, currentToken);
      if (currentToken[0] == "T_RBRACK")
      {
        currentToken = lex.getToken();
        if (currentToken[0] == "T_OF")
        {
          currentToken = lex.getToken();
          array_temp.type = simple(lex, currentToken);
        }
        else syntaxError(currentToken);
      }
      else syntaxError(currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

int Parser::idx(Lexer &lex, vector<string> &currentToken) 
{
  int return_val;
  //change this to return an int instead of void?
  printRule("N_IDX", "T_INTCONST");
  if (currentToken[0] == "T_INTCONST")
  {
    return_val = stoi(currentToken[1]);
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
  return return_val;
}

void Parser::idxRange(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_IDXRANGE", "N_IDX T_DOTDOT N_IDX");
  //get integer from idx, save to temp start
  array_temp.start = idx(lex, currentToken);
  if (currentToken[0] == "T_DOTDOT")
  {
    currentToken = lex.getToken();
    //get integer from idx, save to temp end
    array_temp.end = idx(lex, currentToken);
  }
  else syntaxError(currentToken);
}

int Parser::simple(Lexer &lex, vector<string> &currentToken) 
{
  //change this to return an ENUM of types?
  int return_val;
  //expanded it to catch each type
  if (currentToken[0] == "T_INT")
  {
    return_val = T_INT;
    printRule("N_SIMPLE", currentToken[0]);
    currentToken = lex.getToken();
  }
  else if (currentToken[0] == "T_CHAR") 
  {
    return_val = T_CHAR;
    printRule("N_SIMPLE", currentToken[0]);
    currentToken = lex.getToken();
  }
  else if (currentToken[0] == "T_BOOL")
  {
    return_val = T_BOOL;
    printRule("N_SIMPLE", currentToken[0]);
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
  return return_val;
}

void Parser::procDecPart(Lexer &lex, 
                         vector<string> &currentToken) 
{
  if (currentToken[0] == "T_PROC")
  {
    printRule("N_PROCDECPART",
              "N_PROCDEC T_SCOLON N_PROCDECPART");
    procDec(lex, currentToken);
    if (currentToken[0] == "T_SCOLON")
    {
      currentToken = lex.getToken();
      procDecPart(lex, currentToken);
    }
    else syntaxError(currentToken);
  }
  else printRule("N_PROCDECPART", "epsilon");
}

void Parser::procDec(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_PROCDEC", "N_PROCHDR N_BLOCK");
  procHdr(lex, currentToken);
  block(lex, currentToken);
}

void Parser::procHdr(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_PROCHDR", "T_PROC T_IDENT T_SCOLON");
  if (currentToken[0] == "T_PROC")
  {
    currentToken = lex.getToken();
    if (currentToken[0] == "T_IDENT")
    {
      bool inScope = findEntryInScope(currentToken[1]);
      scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(currentToken[1], T_PROC));
      printAddition(currentToken[1], T_PROC);
      if(inScope)
      {
        //already defined, bail out here
        cout << "Line " << currentToken[2] <<": Multiply defined identifier   "<<endl;
        exit(1);
      }
      currentToken = lex.getToken();
      if (currentToken[0] == "T_SCOLON")
        currentToken = lex.getToken();
      else syntaxError(currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::stmtPart(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_STMTPART", "N_COMPOUND");
  compoundStmt(lex, currentToken);
}

void Parser::compoundStmt(Lexer &lex, 
                          vector<string> &currentToken) 
{
  printRule("N_COMPOUND", "T_BEGIN N_STMT N_STMTLST T_END");
  if (currentToken[0] == "T_BEGIN")
  {
    currentToken = lex.getToken();
    stmt(lex, currentToken);
    stmtLst(lex, currentToken);
    if (currentToken[0] == "T_END")
      currentToken = lex.getToken();
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::stmtLst(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_SCOLON")
  {
    printRule("N_STMTLST", "T_SCOLON N_STMT N_STMTLST");
    currentToken = lex.getToken();
    stmt(lex, currentToken);
    stmtLst(lex, currentToken);
  }
  else printRule("N_STMTLST", "epsilon");
}

void Parser::stmt(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_READ")
  {
    printRule("N_STMT", "N_READ");
    readStmt(lex, currentToken);
  }
  else if (currentToken[0] == "T_WRITE")
  {
   printRule("N_STMT", "N_WRITE");
   writeStmt(lex, currentToken);
  }
  else if (currentToken[0] == "T_IF")
  {
   printRule("N_STMT", "N_CONDITION");
   conditionStmt(lex, currentToken);
  }
  else if (currentToken[0] == "T_WHILE")
  {
   printRule("N_STMT", "N_WHILE");
   whileStmt(lex, currentToken);
  }
  else if (currentToken[0] == "T_BEGIN")
  {
   printRule("N_STMT", "N_COMPOUND");
   compoundStmt(lex, currentToken);
  }
  else 
  {
   printRule("N_STMT", "N_ASSIGN");
   assignStmt(lex, currentToken);
  }
}

void Parser::assignStmt(Lexer &lex, vector<string> &currentToken) {
  printRule("N_ASSIGN", "N_VARIABLE T_ASSIGN N_EXPR");
  variable(lex, currentToken);
  if (currentToken[0] == "T_ASSIGN")
  {
    currentToken = lex.getToken();
    expr(lex, currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::procStmt(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_PROCSTMT", "N_PROCIDENT");
  procIdent(lex, currentToken);
}

void Parser::procIdent(Lexer &lex, vector<string> &currentToken) {
  printRule("N_PROCIDENT", "T_IDENT");
  if (currentToken[0] == "T_IDENT")
  {
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
}

void Parser::readStmt(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_READ", 
            "T_READ T_LPAREN N_INPUTVAR N_INPUTLST T_RPAREN");
  if (currentToken[0] == "T_READ")
  {
    currentToken = lex.getToken();
    if (currentToken[0] == "T_LPAREN")
    {
      currentToken = lex.getToken();
      inputVar(lex, currentToken);
      inputLst(lex, currentToken);
      if (currentToken[0] == "T_RPAREN")
        currentToken = lex.getToken();
      else syntaxError(currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::inputLst(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_COMMA")
  {
    printRule("N_INPUTLST", "T_COMMA N_INPUTVAR N_INPUTLST");
    currentToken = lex.getToken();
    inputVar(lex, currentToken);
    inputLst(lex, currentToken);
  }
  else printRule("N_INPUTLST", "epsilon");
}

void Parser::inputVar(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_INPUTVAR", "N_VARIABLE");
  variable(lex, currentToken);
}

void Parser::writeStmt(Lexer &lex, vector<string> &currentToken) {
  printRule("N_WRITE", 
            "T_WRITE T_LPAREN N_OUTPUT N_OUTPUTLST T_RPAREN");
  if (currentToken[0] == "T_WRITE")
  {
    currentToken = lex.getToken();
    if (currentToken[0] == "T_LPAREN")
    {
      currentToken = lex.getToken();
      output(lex, currentToken);
      outputLst(lex, currentToken);
      if (currentToken[0] == "T_RPAREN")
        currentToken = lex.getToken();
      else syntaxError(currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::outputLst(Lexer &lex, vector<string> &currentToken) {
  if (currentToken[0] == "T_COMMA")
  {
    printRule("N_OUTPUTLST", "T_COMMA N_OUTPUT N_OUTPUTLST");
    currentToken = lex.getToken();
    output(lex, currentToken);
    outputLst(lex, currentToken);
  }
  else printRule("N_OUTPUTLST", "epsilon");
}

void Parser::output(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_OUTPUT", "N_EXPR");
  expr(lex, currentToken);
}

void Parser::conditionStmt(Lexer &lex, 
                           vector<string> &currentToken) 
{
  printRule("N_CONDITION", 
            "T_IF N_EXPR T_THEN N_STMT N_ELSEPART");
  if (currentToken[0] == "T_IF")
  {
    currentToken = lex.getToken();
    expr(lex, currentToken);
    if (currentToken[0] == "T_THEN")
    {
      currentToken = lex.getToken();
      stmt(lex, currentToken);
      elsePart(lex, currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::elsePart(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_ELSE")
  {
    printRule("N_ELSEPART", "T_ELSE N_STMT");
    currentToken = lex.getToken();
    stmt(lex, currentToken);
  }
  else printRule("N_ELSEPART", "epsilon");
}

void Parser::whileStmt(Lexer &lex, vector<string> &currentToken) {
  printRule("N_WHILE", "T_WHILE N_EXPR T_DO N_STMT");
  if (currentToken[0] == "T_WHILE")
  {
    currentToken = lex.getToken();
    expr(lex, currentToken);
    if (currentToken[0] == "T_DO")
    {
      currentToken = lex.getToken();
      stmt(lex, currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::expr(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_EXPR", "N_SIMPLEEXPR N_OPEXPR");
  simpleExpr(lex, currentToken);
  opExpr(lex, currentToken);
}

void Parser::opExpr(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_LT") || 
      (currentToken[0] == "T_LE") || 
      (currentToken[0] == "T_NE") || 
      (currentToken[0] == "T_EQ") || 
      (currentToken[0] == "T_GT") || 
      (currentToken[0] == "T_GE"))
  { 
    printRule("N_OPEXPR", "N_RELOP N_SIMPLEEXPR");
    relOp(lex, currentToken);
    simpleExpr(lex, currentToken);
  }
  else printRule("N_OPEXPR", "epsilon");
}

void Parser::simpleExpr(Lexer &lex, 
                        vector<string> &currentToken) 
{
  printRule("N_SIMPLEEXPR", "N_TERM N_ADDOPLST");
  term(lex, currentToken);
  addOpLst(lex, currentToken);
}

void Parser::addOpLst(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS") ||
      (currentToken[0] == "T_OR"))
  {
    printRule("N_ADDOPLST", "N_ADDOP N_TERM N_ADDOPLST");
    addOp(lex, currentToken);
    term(lex, currentToken);
    addOpLst(lex, currentToken);
  } 
  else printRule("N_ADDOPLST", "epsilon");
}

void Parser::term(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_TERM", "N_FACTOR N_MULTOPLST");
  factor(lex, currentToken);
  multOpLst(lex, currentToken);
}

void Parser::multOpLst(Lexer &lex, vector<string> &currentToken) {
  if ((currentToken[0] == "T_MULT") || 
      (currentToken[0] == "T_DIV") || 
      (currentToken[0] == "T_AND")) 
  {
    printRule("N_MULTOPLST", "N_MULTOP N_FACTOR N_MULTOPLST");
    multOp(lex, currentToken);
    factor(lex, currentToken);
    multOpLst(lex, currentToken);
  }
  else printRule("N_MULTOPLST", "epsilon");
}

void Parser::factor(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_LPAREN")
  {
    printRule("N_FACTOR", "T_LPAREN N_EXPR T_RPAREN");
    currentToken = lex.getToken();
    expr(lex, currentToken);
    if (currentToken[0] == "T_RPAREN")
      currentToken = lex.getToken();
    else syntaxError(currentToken);
  }
  else 
  {
    if (currentToken[0] == "T_NOT")
    {
      printRule("N_FACTOR", "T_NOT N_FACTOR");
      currentToken = lex.getToken();
      factor(lex, currentToken);
    }
    else
    {
      if ((currentToken[0] == "T_INTCONST") ||
          (currentToken[0] == "T_CHARCONST") ||
          (currentToken[0] == "T_TRUE") || 
          (currentToken[0] == "T_FALSE"))
      {
        printRule("N_FACTOR", "N_CONST");
        constant(lex, currentToken);
      }
      else
      {
        printRule("N_FACTOR", "N_SIGN N_VARIABLE");
        sign(lex, currentToken);
        variable(lex, currentToken);
      }
    }
  }
}

void Parser::sign(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS"))
  {
    printRule("N_SIGN", currentToken[0]); 
    currentToken = lex.getToken();
  }
  else printRule("N_SIGN", "epsilon");
}

void Parser::addOp(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS") ||
      (currentToken[0] == "T_OR"))
  { 
    printRule("N_ADDOP", currentToken[0]);  
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
}

void Parser::multOp(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_MULT") || 
      (currentToken[0] == "T_DIV") || 
      (currentToken[0] == "T_AND"))
  {
    printRule("N_MULTOP", currentToken[0]);  
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
}

void Parser::relOp(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_LT") || 
      (currentToken[0] == "T_LE") || 
      (currentToken[0] == "T_NE") || 
      (currentToken[0] == "T_EQ") || 
      (currentToken[0] == "T_GT") || 
      (currentToken[0] == "T_GE"))
  {
    printRule("N_RELOP", currentToken[0]);   
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
}

void Parser::variable(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_VARIABLE", "T_IDENT N_IDXVAR");
  if (currentToken[0] == "T_IDENT")
  {
    if(!findEntryInAnyScope(currentToken[1]))
    {
        cout << "Line " << currentToken[2] <<": Undefined identifier"<<endl;
        exit(1);
    }
    currentToken = lex.getToken();
    idxVar(lex, currentToken);
  }
  else syntaxError(currentToken);
}

void Parser::idxVar(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_LBRACK")
  {
    printRule("N_IDXVAR", "T_LBRACK N_EXPR T_RBRACK");
    currentToken = lex.getToken();
    expr(lex, currentToken);
    if (currentToken[0] == "T_RBRACK")
      currentToken = lex.getToken();
    else syntaxError(currentToken);
  }
  else printRule("N_IDXVAR", "epsilon");
}

void Parser::constant(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_TRUE") ||
      (currentToken[0] == "T_FALSE"))
  {
    printRule("N_CONST", "N_BOOLCONST");
    boolConst(lex, currentToken);
  }
  else if ((currentToken[0] == "T_INTCONST") ||
           (currentToken[0] == "T_CHARCONST"))
       {
         printRule("N_CONST", currentToken[0]);
         currentToken = lex.getToken();
       }
       else syntaxError(currentToken);
}

void Parser::boolConst(Lexer &lex, vector<string> &currentToken) {
  if ((currentToken[0] == "T_TRUE") ||
      (currentToken[0] == "T_FALSE"))
  {
    printRule("N_BOOLCONST", currentToken[0]);  
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
}










    
