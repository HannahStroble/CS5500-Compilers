#include "parser.h"

// Static Parser variables
bool Parser::print_productions_flag = false;
bool Parser::print_symbolTableMgt_flag = true;


void Parser::printRule(const string lhs, const string rhs)
{
  if (print_productions_flag)
    cout << lhs << " -> " << rhs << endl; 
  return;
}

void Parser::syntaxError(const vector<string> currentToken) const 
{
  printError(currentToken, "syntax error");  
}

void Parser::printError(const vector<string> currentToken,
                        const string msg) const 
{
  cout << "Line " << currentToken[2]
       << ": " << msg << endl;
  exit(1);
}


void Parser::prog(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_PROG", "N_PROGLBL T_IDENT T_SCOLON N_BLOCK T_DOT"); 
  progLbl(lex, currentToken);
  if (currentToken[0] == "T_IDENT") 
  {
    TYPE_INFO info = {PROGRAM, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    prSymbolTableAddition(currentToken[1], info);
    scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(currentToken[1],info));
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
  beginScope();
}
 
void Parser::block(Lexer &lex, vector<string> &currentToken) 
{
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
  // ident and identLst put identifers on variableNames list
  ident(lex, currentToken);
  identLst(lex, currentToken);
  if (currentToken[0] == "T_COLON")
  {
    currentToken = lex.getToken();
    TYPE_INFO info = identType(lex, currentToken);
    // Add to symbol table each identifier that is on
    // variableNames list, assigning it type info
    for (std::list<string>::iterator
         it = variableNames.begin();
         it != variableNames.end(); it++) 
    {
      string varName = string(*it);
      prSymbolTableAddition(varName, info);
      bool success = scopeStack.top().addEntry
       (SYMBOL_TABLE_ENTRY(varName, info));
      if (!success)
        printError(currentToken, MULTIPLY_DEFINED);
    }
  }
  else syntaxError(currentToken);
  variableNames.clear();
}

void Parser::ident(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_IDENT", "T_IDENT");
  if (currentToken[0] == "T_IDENT")
  {
    // Add it to variableNames list necessary in case
    // ident called from varDec
    variableNames.push_back(currentToken[1]);
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

TYPE_INFO Parser::identType(Lexer &lex, vector<string> &currentToken)
{
  TYPE_INFO info;

  if (currentToken[0] == "T_ARRAY")
  {
    printRule("N_TYPE", "N_ARRAY");
    info = array(lex, currentToken);
  }
  else 
  {
    printRule("N_TYPE", "N_SIMPLE");
    info = simple(lex, currentToken);
  }
  return(info);
}

TYPE_INFO Parser::array(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;

  printRule("N_ARRAY", "T_ARRAY T_LBRACK N_IDXRANGE T_RBRACK T_OF N_SIMPLE");
  if (currentToken[0] == "T_ARRAY")
  {
    currentToken = lex.getToken();
    if (currentToken[0] == "T_LBRACK")
    {
      currentToken = lex.getToken();
      TYPE_INFO indexInfo = idxRange(lex, currentToken);
      if (currentToken[0] == "T_RBRACK")
      {
        currentToken = lex.getToken();
        if (currentToken[0] == "T_OF")
        {
          currentToken = lex.getToken();
          // Collect the type information
          TYPE_INFO baseTypeInfo = simple(lex, currentToken);
          info.type = ARRAY;
          info.baseType = baseTypeInfo.type;
          info.startIndex = indexInfo.startIndex;
          info.endIndex = indexInfo.endIndex;
        }
        else syntaxError(currentToken);
      }
      else syntaxError(currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
  return(info);
}

int Parser::idx(Lexer &lex, vector<string> &currentToken) 
{
  int val;

  printRule("N_IDX", "T_INTCONST");
  if (currentToken[0] == "T_INTCONST")
  {
    val = atoi(currentToken[1].c_str());
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
  return(val);
}

TYPE_INFO Parser::idxRange(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;

  printRule("N_IDXRANGE", "N_IDX T_DOTDOT N_IDX");
  info.type = INDEX_RANGE;
  info.startIndex = idx(lex, currentToken);
  if (currentToken[0] == "T_DOTDOT")
  {
    currentToken = lex.getToken();
    info.endIndex = idx(lex, currentToken);
  }
  else syntaxError(currentToken);
  return(info);
}

TYPE_INFO Parser::simple(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;
  if ((currentToken[0] == "T_INT") ||
      (currentToken[0] == "T_CHAR") || 
      (currentToken[0] == "T_BOOL"))
  {
    printRule("N_SIMPLE", currentToken[0]);
    if (currentToken[0] == "T_INT")
      info.type = INT;
    else if (currentToken[0] == "T_CHAR")
      info.type = CHAR;
    else info.type = BOOL;
    info.startIndex = NOT_APPLICABLE;
    info.endIndex = NOT_APPLICABLE;
    info.baseType = NOT_APPLICABLE;
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
  return(info);
}

void Parser::procDecPart(Lexer &lex, vector<string> &currentToken) 
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
      TYPE_INFO info = {PROCEDURE, NOT_APPLICABLE,
                        NOT_APPLICABLE, NOT_APPLICABLE};
      prSymbolTableAddition(currentToken[1], info);
      bool success = scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(currentToken[1], info));
      if (!success) 
        printError(currentToken, MULTIPLY_DEFINED);
      currentToken = lex.getToken();
      if (currentToken[0] == "T_SCOLON")
        currentToken = lex.getToken();
      else syntaxError(currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
  beginScope();
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
  else {
         printRule("N_STMT", "N_ASSIGN");
         assignStmt(lex, currentToken);
       }
}

void Parser::assignStmt(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO left_h, right_h;
  printRule("N_ASSIGN", "N_VARIABLE T_ASSIGN N_EXPR");
  left_h = variable(lex, currentToken);
  if(left_h.type == PROCEDURE || left_h.type == PROGRAM)
  {
    cout << "Line " << currentToken[2] << ": Procedure/variable mismatch" << endl;
    exit(0);
  }
  if (currentToken[0] == "T_ASSIGN")
  {
    currentToken = lex.getToken();
    right_h = expr(lex, currentToken);
    if(right_h.type == PROCEDURE || right_h.type == PROGRAM)
    {
      cout << "Line " << currentToken[2] << ": Procedure/variable mismatch" << endl;
      exit(0);
    }
    else if (left_h.type != right_h.type)
    {
      cout << "Line " << currentToken[2] << ": Expression must be of same type as variable" << endl;
      exit(0);
    }
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
    TYPE_INFO typeInfo = findEntryInAnyScope(currentToken[1]);
    if (typeInfo.type == UNDEFINED) 
      printError(currentToken, UNDEFINED_IDENT);
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
}

void Parser::readStmt(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_READ", "T_READ T_LPAREN N_INPUTVAR N_INPUTLST T_RPAREN");
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
  TYPE_INFO typeInfo;
  printRule("N_INPUTVAR", "N_VARIABLE");
  typeInfo = variable(lex, currentToken);
  if (tmp_info.type != T_INT && tmp_info.type != T_CHAR)
  {
    cout << "Line " << currentToken[2] << ": Input variable must be of type integer or char" << endl;
    exit(0);
  }
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

void Parser::outputLst(Lexer &lex, vector<string> &currentToken) 
{
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
  TYPE_INFO typeInfo;
  printRule("N_OUTPUT", "N_EXPR");
  typeInfo = expr(lex, currentToken);
  if (tmp_info.type != T_INT && tmp_info.type != T_CHAR)
  {
    cout << "Line " << currentToken[2] << ": Output expression must be of type integer or char" << endl;
    exit(0);
  }
}

void Parser::conditionStmt(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO typeInfo;
  printRule("N_CONDITION", 
            "T_IF N_EXPR T_THEN N_STMT N_ELSEPART");
  if (currentToken[0] == "T_IF")
  {
    currentToken = lex.getToken();
    typeInfo = expr(lex, currentToken);
    if(typeInfo.type != BOOL)
    {
      cout << "Line " << currentToken[2] << ": Expression must be of type boolean" << endl;
      exit(0);
    }
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

void Parser::whileStmt(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO typeInfo;
  printRule("N_WHILE", "T_WHILE N_EXPR T_DO N_STMT");
  if (currentToken[0] == "T_WHILE")
  {
    currentToken = lex.getToken();
    typeInfo = expr(lex, currentToken);
    if(typeInfo.type != BOOL)
    {
      cout << "Line " << currentToken[2] << ": Expression must be of type boolean" << endl;
      exit(0);
    }
    if (currentToken[0] == "T_DO")
    {
      currentToken = lex.getToken();
      stmt(lex, currentToken);
    }
    else syntaxError(currentToken);
  }
  else syntaxError(currentToken);
}

TYPE_INFO Parser::expr(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO simpleType, opType, returnType; 
  printRule("N_EXPR", "N_SIMPLEEXPR N_OPEXPR");
  simpleType = simpleExpr(lex, currentToken);
  opType = opExpr(lex, currentToken);

  if(opExpr != NOT_APPLICABLE)
  {
    returnType = {BOOL, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    return returnType;
  }
  else if (simpleType.type != opType.type)
  {
    cout << "Line: " << currentToken[2] << ": Expressions must both be int, or both char, or both boolean" << endl;
    exit(0);  
  }
  else
  {
    return simpleType;
  }

}

TYPE_INFO Parser::opExpr(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO returnType, simpleType;
  if ((currentToken[0] == "T_LT") || 
      (currentToken[0] == "T_LE") || 
      (currentToken[0] == "T_NE") || 
      (currentToken[0] == "T_EQ") || 
      (currentToken[0] == "T_GT") || 
      (currentToken[0] == "T_GE"))
  { 
    printRule("N_OPEXPR", "N_RELOP N_SIMPLEEXPR");
    relOp(lex, currentToken);
    simpleType = simpleExpr(lex, currentToken);
    return simpleType;
  }
  else
  { 
    returnType = {NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    printRule("N_OPEXPR", "epsilon");
  }
}

TYPE_INFO Parser::simpleExpr(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO termType;
  printRule("N_SIMPLEEXPR", "N_TERM N_ADDOPLST");
  term(lex, currentToken);
  addOpLst(lex, currentToken);
  return termType;
}

TYPE_INFO Parser::addOpLst(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO opType, termType, returnType;
  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS") ||
      (currentToken[0] == "T_OR"))
  {
    printRule("N_ADDOPLST", "N_ADDOP N_TERM N_ADDOPLST");
    addOp(lex, currentToken);
    termType = term(lex, currentToken);
    addOpLst(lex, currentToken);
  } 
  else 
  {
    returnType = {NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    printRule("N_ADDOPLST", "epsilon");
  }
}

TYPE_INFO Parser::term(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO factorType;
  printRule("N_TERM", "N_FACTOR N_MULTOPLST");
  factorType = factor(lex, currentToken);
  multOpLst(lex, currentToken);
  return factorType;
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
  TYPE_INFO variableType, constantType, exprType, returnType;
  bool hasSign;
  if (currentToken[0] == "T_LPAREN")
  {
    printRule("N_FACTOR", "T_LPAREN N_EXPR T_RPAREN");
    currentToken = lex.getToken();
    exprType = expr(lex, currentToken);
    if (currentToken[0] == "T_RPAREN")
      currentToken = lex.getToken();
    else syntaxError(currentToken);
    return exprType;
  }
  else 
  {
    if (currentToken[0] == "T_NOT")
    {
      printRule("N_FACTOR", "T_NOT N_FACTOR");
      currentToken = lex.getToken();
      factor(lex, currentToken);
      returnType = {BOOL, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
      return returnType;
    }
    else
    {
      if ((currentToken[0] == "T_INTCONST") ||
          (currentToken[0] == "T_CHARCONST") ||
          (currentToken[0] == "T_TRUE") || 
          (currentToken[0] == "T_FALSE"))
      {
        printRule("N_FACTOR", "N_CONST");
        constantType = constant(lex, currentToken);
        return constantType;
      }
      else
      {
        printRule("N_FACTOR", "N_SIGN N_VARIABLE");
        hasSign = sign(lex, currentToken);
        variableType = variable(lex, currentToken);
        if (hasSign && variableType.type != INTEGER)
        {
          cout << "Line " << currentToken[2] << ": Expression must be of type integer" << endl;
          exit(0);
        }
        return variableType;
      }
    }
  }
}

bool Parser::sign(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS"))
  {
    printRule("N_SIGN", currentToken[0]); 
    currentToken = lex.getToken();
    return true;
  }
  else
  {
    printRule("N_SIGN", "epsilon");
    return false;
  }
}

TYPE_INFO Parser::addOp(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO returnType;
  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS") ||
      (currentToken[0] == "T_OR"))
  { 
    printRule("N_ADDOP", currentToken[0]);  
    if(currentToken[0] == "T_PLUS" || currentToken[0] == "T_MINUS")
    {
      returnType = {INTEGER, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    }
    else
    {
      returnType = {BOOL, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    }
    currentToken = lex.getToken();
    return returnType;
  }
  else syntaxError(currentToken);
}

TYPE_INFO Parser::multOp(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO returnType;
  if ((currentToken[0] == "T_MULT") || 
      (currentToken[0] == "T_DIV") || 
      (currentToken[0] == "T_AND"))
  {
    printRule("N_MULTOP", currentToken[0]); 
    if(currentToken[0] == "T_MULT" || currentToken[0] == "T_DIV")
    {
      returnType = {INTEGER, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    }
    else
    {
      returnType = {BOOL, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
    }
    currentToken = lex.getToken();
    return returnType;
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

TYPE_INFO Parser::variable(Lexer &lex, vector<string> &currentToken) 
{
  bool isArray;
  printRule("N_VARIABLE", "T_IDENT N_IDXVAR");
  if (currentToken[0] == "T_IDENT")
  {
    TYPE_INFO typeInfo = findEntryInAnyScope(currentToken[1]);
    if (typeInfo.type == UNDEFINED)
      printError(currentToken, UNDEFINED_IDENT);
    if (typeInfo.type == PROCEDURE)
      printError(currentToken, "Procedure/variable mismatch");
    currentToken = lex.getToken();
    isArray = idxVar(lex, currentToken);
    if(typeInfo.type == ARRAY && !isArray)
      printError(currentToken, "Array variable must be indexed")
    if(isArray)
    {
      if(typeInfo.type != ARRAY)
        printError(currentToken, "Indexed variable must be of array type")
    }
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

void Parser::beginScope() 
{
  scopeStack.push(SYMBOL_TABLE());
  if (print_symbolTableMgt_flag) 
    printf("\n\n>>> Entering new scope...\n");
}

void Parser::endScope() 
{
  scopeStack.pop();
  if (print_symbolTableMgt_flag) 
    printf("\n<<< Exiting scope...\n");
}

void Parser::prSymbolTableAddition(const string identName, 
                                   const TYPE_INFO typeInfo) 
{
  if (print_symbolTableMgt_flag) 
  {
   char *cstr = new char[identName.length() + 1];
   strcpy(cstr, identName.c_str());
   printf("\n+++ Adding %s to symbol table with type ", cstr);
   delete [] cstr;
   switch (typeInfo.type) {
	case PROGRAM	: printf("PROGRAM\n"); break;
	case PROCEDURE	: printf("PROCEDURE\n"); break;
	case INT		: printf("INTEGER\n"); break;
	case CHAR		: printf("CHAR\n"); break;
	case BOOL		: printf("BOOLEAN\n"); break;
	case ARRAY		: printf("ARRAY ");
				  printf("%d .. %d OF ",
				         typeInfo.startIndex, 
				         typeInfo.endIndex);
				  switch (typeInfo.baseType) {
				    case INT : printf("INTEGER\n"); 
                                     break;
				    case CHAR: printf("CHAR\n"); 
                                     break;
				    case BOOL: printf("BOOLEAN\n"); 
                                     break;
				    default  : printf("UNKNOWN\n"); 
                                     break;
				  }
				  break;
	default 		: printf("UNKNOWN\n"); break;
   }
  }
}

TYPE_INFO Parser::findEntryInAnyScope(const string theName) 
{
  TYPE_INFO info = {UNDEFINED, NOT_APPLICABLE, NOT_APPLICABLE,
                    NOT_APPLICABLE};
  if (scopeStack.empty( )) return(info);
  info = scopeStack.top().findEntry(theName);
  if (info.type != UNDEFINED)
    return(info);
  else { // check in "next higher" scope
	   SYMBOL_TABLE symbolTable = scopeStack.top( );
	   scopeStack.pop( );
	   info = findEntryInAnyScope(theName);
	   scopeStack.push(symbolTable); // restore the stack
	   return(info);
  }
}

void Parser::cleanUp() 
{
  if (scopeStack.empty()) 
    return;
  else 
  {
    scopeStack.pop();
    cleanUp();
  }
}








    