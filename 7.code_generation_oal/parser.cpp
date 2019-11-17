#include "parser.h"

// Static Parser variables
bool Parser::print_productions_flag = false;
bool Parser::print_symbolTableMgt_flag = false;

const int display_size = 20;
const int stack_size = 500;

int label = 1;
int nest_level = 0;
int stack_label;
int main_label;
int code_label;


ostringstream oal_prog;
int mem_words = 0;


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
      //init block
      mem_words = 0;

      //grab labels and increment as you go
      stack_label = label++;
      code_label  = label++;
      main_label  = label++;

      cout << "stack_label :" << stack_label << endl;
      cout << "code_label  :" << code_label << endl;
      cout << "main_label  :" << main_label << endl;
      //init block with file offsets
      oal_prog << "  init L.0 " << display_size << ", L." << stack_label << ", L." << code_label << ", L." << main_label << endl;
      oal_prog << "L.0:" << endl;

      currentToken = lex.getToken();
      block(lex, currentToken);
      if (currentToken[0] == "T_DOT")
      {
        oal_prog << "  halt" << endl;
        oal_prog << "L." << stack_label << ":" << endl;
        oal_prog << "  bss " << stack_size << endl;
        oal_prog << "  end" << endl;
        currentToken = lex.getToken();
      }
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
  printRule("N_BLOCK", "N_VARDECPART N_PROCDECPART N_STMTPART");
  varDecPart(lex, currentToken);
  //after we know how many words we have we add bss for it
  if(nest_level == 0)
  {
    oal_prog << "  bss " << display_size + mem_words << endl;
    oal_prog << "L." << code_label << ":" << endl;
  }
  else
  {
    //Get words of memory for scope here
    //  *add count in var dec rule?
    //  *add query function to scope?
  }
  procDecPart(lex, currentToken);
  stmtPart(lex, currentToken);
  endScope();
}
 
void Parser::varDecPart(Lexer &lex, vector<string> &currentToken) 
{
  if (currentToken[0] == "T_VAR") 
  {
    printRule("N_VARDECPART", "T_VAR N_VARDEC T_SCOLON N_VARDECLST");
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
    for (std::list<string>::iterator it = variableNames.begin(); it != variableNames.end(); it++) 
    {
      string varName = string(*it);
      prSymbolTableAddition(varName, info);
      bool success = scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(varName, info));
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

TYPE_INFO Parser::identType(Lexer &lex, 
                            vector<string> &currentToken)
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

TYPE_INFO Parser::array(Lexer &lex, 
                        vector<string> &currentToken) 
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
    if (info.endIndex < info.startIndex)
      printError(currentToken, ERR_START_INDEX_MUST_BE_LE_END_INDEX);
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

void Parser::procDecPart(Lexer &lex, 
                         vector<string> &currentToken) 
{
  if (currentToken[0] == "T_PROC")
  {
    printRule("N_PROCDECPART", "N_PROCDEC T_SCOLON N_PROCDECPART");
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
      TYPE_INFO info = {PROCEDURE, NOT_APPLICABLE, NOT_APPLICABLE, NOT_APPLICABLE};
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
  if(nest_level == 0)
  {
    oal_prog << "L." << main_label << ":" << endl;
  }
  else
  {
    //figure this out later
  }
  printRule("N_STMTPART", "N_COMPOUND");
  compoundStmt(lex, currentToken);
}

void Parser::compoundStmt(Lexer &lex, vector<string> &currentToken) 
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
    TYPE_INFO typeInfo = findEntryInAnyScope(currentToken[1]);
    switch (typeInfo.type) 
    {
      case UNDEFINED : 
        printError(currentToken, UNDEFINED_IDENT);
        break;
      case PROCEDURE :
        printRule("N_STMT", "N_PROCSTMT");
        procStmt(lex, currentToken);
        break;
      default :
        printRule("N_STMT", "N_ASSIGN");
        assignStmt(lex, currentToken);
    }
  }
}

void Parser::assignStmt(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO lhsType, rhsType;

  printRule("N_ASSIGN", "N_VARIABLE T_ASSIGN N_EXPR");
  lhsType = variable(lex, currentToken);
  if (currentToken[0] == "T_ASSIGN")
  {
    currentToken = lex.getToken();
    rhsType = expr(lex, currentToken);
    if (lhsType.type != rhsType.type)
      printError(currentToken, ERR_EXPR_MUST_BE_SAME_AS_VAR);
  }
  else syntaxError(currentToken);
}

void Parser::procStmt(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_PROCSTMT", "N_PROCIDENT");
  procIdent(lex, currentToken);
}

void Parser::procIdent(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_PROCIDENT", "T_IDENT");
  if (currentToken[0] == "T_IDENT")
  {
    // Actually, it will catch undefined in stmt (don't really
    // need this check again here)
    TYPE_INFO typeInfo = findEntryInAnyScope(currentToken[1]);
    if (typeInfo.type == UNDEFINED) 
      printError(currentToken, UNDEFINED_IDENT);
    if (typeInfo.type != PROCEDURE)
      printError(currentToken, ERR_PROCEDURE_VAR_MISMATCH);
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
  printRule("N_INPUTVAR", "N_VARIABLE");
  TYPE_INFO info = variable(lex, currentToken);
  if ((info.type != INT) && (info.type != CHAR))
    printError(currentToken, ERR_INPUT_VAR_MUST_BE_INT_OR_CHAR);
}

void Parser::writeStmt(Lexer &lex, vector<string> &currentToken) {
  printRule("N_WRITE", "T_WRITE T_LPAREN N_OUTPUT N_OUTPUTLST T_RPAREN");
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
  printRule("N_OUTPUT", "N_EXPR");
  TYPE_INFO info = expr(lex, currentToken);
  if ((info.type != INT) && (info.type != CHAR))
    printError(currentToken, 
               ERR_OUTPUT_VAR_MUST_BE_INT_OR_CHAR);
}

void Parser::conditionStmt(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_CONDITION", "T_IF N_EXPR T_THEN N_STMT N_ELSEPART");
  if (currentToken[0] == "T_IF")
  {
    currentToken = lex.getToken();
    TYPE_INFO info = expr(lex, currentToken);
    if (info.type != BOOL)
      printError(currentToken, ERR_EXPR_MUST_BE_BOOL);
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
  printRule("N_WHILE", "T_WHILE N_EXPR T_DO N_STMT");
  if (currentToken[0] == "T_WHILE")
  {
    currentToken = lex.getToken();
    TYPE_INFO info = expr(lex, currentToken);
    if (info.type != BOOL)
      printError(currentToken, ERR_EXPR_MUST_BE_BOOL);
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
  TYPE_INFO lhsType, rhsType;

  printRule("N_EXPR", "N_SIMPLEEXPR N_OPEXPR");
  lhsType = simpleExpr(lex, currentToken);
  rhsType = opExpr(lex, currentToken);

  if (rhsType.type != NOT_APPLICABLE)
  {
    if (lhsType.type != rhsType.type)
      printError(currentToken, ERR_EXPRS_MUST_BOTH_BE_SAME_TYPE);

    // Type of this expr is type of simpleExpr
    // or just bool if there was a relop after
    // the simpleExpr
    lhsType.type = BOOL; 
    lhsType.startIndex = NOT_APPLICABLE;
    lhsType.endIndex = NOT_APPLICABLE;
    lhsType.baseType = NOT_APPLICABLE;
  }
  return(lhsType);
}

TYPE_INFO Parser::opExpr(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;

  if ((currentToken[0] == "T_LT") || 
      (currentToken[0] == "T_LE") || 
      (currentToken[0] == "T_NE") || 
      (currentToken[0] == "T_EQ") || 
      (currentToken[0] == "T_GT") || 
      (currentToken[0] == "T_GE"))
  { 
    printRule("N_OPEXPR", "N_RELOP N_SIMPLEEXPR");
    relOp(lex, currentToken);
    info = simpleExpr(lex, currentToken);
  }
  else 
  {
    printRule("N_OPEXPR", "epsilon");
    info.type = NOT_APPLICABLE; 
    info.startIndex = NOT_APPLICABLE;
    info.endIndex = NOT_APPLICABLE;
    info.baseType = NOT_APPLICABLE;
  }
  return(info);
}

TYPE_INFO Parser::simpleExpr(Lexer &lex, vector<string> &currentToken) 
{
  printRule("N_SIMPLEEXPR", "N_TERM N_ADDOPLST");
  TYPE_INFO info = term(lex, currentToken);
  addOpLst(lex, currentToken);
  return(info);
}

void Parser::addOpLst(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS") ||
      (currentToken[0] == "T_OR"))
  {
    printRule("N_ADDOPLST", "N_ADDOP N_TERM N_ADDOPLST");
    int opType = addOp(lex, currentToken);
    TYPE_INFO info = term(lex, currentToken);
    if ((opType == LOGICAL_OP) && (info.type != BOOL))
      printError(currentToken, ERR_EXPR_MUST_BE_BOOL);
    if ((opType == ARITHMETIC_OP) && (info.type != INT))
      printError(currentToken, ERR_EXPR_MUST_BE_INT);
    addOpLst(lex, currentToken);
  } 
  else printRule("N_ADDOPLST", "epsilon");
}

TYPE_INFO Parser::term(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;

  printRule("N_TERM", "N_FACTOR N_MULTOPLST");
  info = factor(lex, currentToken);
  multOpLst(lex, currentToken);
  return(info);
}

void Parser::multOpLst(Lexer &lex, vector<string> &currentToken) 
{
  if ((currentToken[0] == "T_MULT") || 
      (currentToken[0] == "T_DIV")  || 
      (currentToken[0] == "T_AND")) 
  {
    printRule("N_MULTOPLST", "N_MULTOP N_FACTOR N_MULTOPLST");
    int opType = multOp(lex, currentToken);
    TYPE_INFO info = factor(lex, currentToken);
    if ((opType == LOGICAL_OP) && (info.type != BOOL))
      printError(currentToken, ERR_EXPR_MUST_BE_BOOL);
    if ((opType == ARITHMETIC_OP) && (info.type != INT))
      printError(currentToken, ERR_EXPR_MUST_BE_INT);
    multOpLst(lex, currentToken);
  }
  else printRule("N_MULTOPLST", "epsilon");
}

TYPE_INFO Parser::factor(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;

  if (currentToken[0] == "T_LPAREN")
  {
    printRule("N_FACTOR", "T_LPAREN N_EXPR T_RPAREN");
    currentToken = lex.getToken();
    info = expr(lex, currentToken);
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
      info = factor(lex, currentToken);
      if (info.type != BOOL)
        printError(currentToken, ERR_EXPR_MUST_BE_BOOL);
    }
    else
    {
      if ((currentToken[0] == "T_INTCONST")   ||
          (currentToken[0] == "T_CHARCONST")  ||
          (currentToken[0] == "T_TRUE")       || 
          (currentToken[0] == "T_FALSE"))
      {
        printRule("N_FACTOR", "N_CONST");
        info = constant(lex, currentToken);
      }
      else
      {
        printRule("N_FACTOR", "N_SIGN N_VARIABLE");
        int signType = sign(lex, currentToken);
        info = variable(lex, currentToken);
        if ((signType != NO_SIGN) && (info.type != INT))
          printError(currentToken, ERR_EXPR_MUST_BE_INT);
      }
    }
  }
  return(info);
}

int Parser::sign(Lexer &lex, vector<string> &currentToken) 
{
  int signType = NO_SIGN;

  if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS"))
  {
    printRule("N_SIGN", currentToken[0]);
    if (currentToken[0] == "T_PLUS")
      signType = POSITIVE;
    else signType = NEGATIVE; 
    currentToken = lex.getToken();
  }
  else printRule("N_SIGN", "epsilon");
  return(signType);
}

int Parser::addOp(Lexer &lex, vector<string> &currentToken) 
{
  int opType;

  if ((currentToken[0] == "T_PLUS")  || 
      (currentToken[0] == "T_MINUS") ||
      (currentToken[0] == "T_OR"))
  { 
    printRule("N_ADDOP", currentToken[0]);
    if (currentToken[0] == "T_OR")
      opType = LOGICAL_OP;
    else opType = ARITHMETIC_OP;  
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
  return(opType);
}

int Parser::multOp(Lexer &lex, vector<string> &currentToken) 
{
  int opType;

  if ((currentToken[0] == "T_MULT") || 
      (currentToken[0] == "T_DIV")  || 
      (currentToken[0] == "T_AND"))
  {
    printRule("N_MULTOP", currentToken[0]);
    if (currentToken[0] == "T_AND")
      opType = LOGICAL_OP;
    else opType = ARITHMETIC_OP;
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
  return(opType);
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

TYPE_INFO Parser::variable(Lexer &lex, 
                           vector<string> &currentToken) 
{
  TYPE_INFO typeInfo;
  bool isArrayElement;

  printRule("N_VARIABLE", "T_IDENT N_IDXVAR");
  if (currentToken[0] == "T_IDENT")
  {
    typeInfo = findEntryInAnyScope(currentToken[1]);
    if (typeInfo.type == UNDEFINED) 
      printError(currentToken, UNDEFINED_IDENT);
    if (typeInfo.type == PROCEDURE)
      printError(currentToken, ERR_PROCEDURE_VAR_MISMATCH);
    currentToken = lex.getToken();
    isArrayElement = idxVar(lex, currentToken);
    if ((typeInfo.type == ARRAY) && !isArrayElement)
      printError(currentToken, ERR_ARRAY_VAR_MUST_BE_INDEXED);
    if (isArrayElement)
    {
      if (typeInfo.type != ARRAY)
        printError(currentToken, ERR_INDEX_VAR_MUST_BE_ARRAY);
      typeInfo.type = typeInfo.baseType;
      typeInfo.startIndex = NOT_APPLICABLE;
      typeInfo.endIndex = NOT_APPLICABLE;
      typeInfo.baseType = NOT_APPLICABLE;
    }
  }
  else syntaxError(currentToken);
  return(typeInfo);
}

bool Parser::idxVar(Lexer &lex, vector<string> &currentToken) 
{
  bool isArrayElement = false;

  if (currentToken[0] == "T_LBRACK")
  {
    printRule("N_IDXVAR", "T_LBRACK N_EXPR T_RBRACK");
    isArrayElement = true;
    currentToken = lex.getToken();
    TYPE_INFO info = expr(lex, currentToken);
    if (info.type != INT)
      printError(currentToken, ERR_INDEX_EXPR_MUST_BE_INT);
    if (currentToken[0] == "T_RBRACK")
      currentToken = lex.getToken();
    else syntaxError(currentToken);
  }
  else printRule("N_IDXVAR", "epsilon");
  return(isArrayElement);
}

TYPE_INFO Parser::constant(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;

  if ((currentToken[0] == "T_TRUE") ||
      (currentToken[0] == "T_FALSE"))
  {
    printRule("N_CONST", "N_BOOLCONST");
    info = boolConst(lex, currentToken);
  }
  else if ((currentToken[0] == "T_INTCONST") ||
           (currentToken[0] == "T_CHARCONST"))
       {
         printRule("N_CONST", currentToken[0]);
         if (currentToken[0] == "T_INTCONST")
           info.type = INT;
         else info.type = CHAR; 
         info.startIndex = NOT_APPLICABLE;
         info.endIndex = NOT_APPLICABLE;
         info.baseType = NOT_APPLICABLE;  
         currentToken = lex.getToken();
       }
       else syntaxError(currentToken);
  return(info);
}

TYPE_INFO Parser::boolConst(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO info;

  if ((currentToken[0] == "T_TRUE") ||
      (currentToken[0] == "T_FALSE"))
  {
    printRule("N_BOOLCONST", currentToken[0]);
    info.type = BOOL;
    info.startIndex = NOT_APPLICABLE;
    info.endIndex = NOT_APPLICABLE;
    info.baseType = NOT_APPLICABLE;  
    currentToken = lex.getToken();
  }
  else syntaxError(currentToken);
  return(info);
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
    switch (typeInfo.type) 
    {
      case PROGRAM: 
        printf("PROGRAM\n"); 
        break;
      case PROCEDURE:
        printf("PROCEDURE\n"); 
        break;
      case INT:
        printf("INTEGER\n"); 
        break;
      case CHAR: 
        printf("CHAR\n"); 
        break;
      case BOOL: 
        printf("BOOLEAN\n"); 
        break;
      case ARRAY:
        printf("ARRAY ");
			  printf("%d .. %d OF ", typeInfo.startIndex, typeInfo.endIndex);
			  switch (typeInfo.baseType) 
        {
		      case INT: 
            printf("INTEGER\n"); 
            break;
			    case CHAR: 
            printf("CHAR\n"); 
            break;
          case BOOL: 
            printf("BOOLEAN\n"); 
            break;
          default: 
            printf("UNKNOWN\n"); 
            break;
				}
			  break;
      default: 
        printf("UNKNOWN\n"); 
        break;
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








    