#include "parser.h"
#include "SymbolTable.h"

// Static Parser variable
bool Parser::print_productions_flag = true; 
bool Parser::print_additions_flag = false; 
bool Parser::suppressTokenOutput = true;

std::vector<string> ident_tmp;
TYPE_INFO array_temp;
TYPE_INFO tmp_simple;
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
  else 
  {
    scopeStack.pop();
    cleanUp();
  }
}

bool Parser::findEntryInScope(const string the_name)
{
  if (scopeStack.empty()) return(false);
  TYPE_INFO found = scopeStack.top().findEntry(the_name);
  if(found.type == UNDEFINED)
    return false;
  else
    return true;
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
  bool isFound;
  if (scopeStack.empty()) 
    return false;
  TYPE_INFO found = scopeStack.top().findEntry(the_name);
  if (found.type != UNDEFINED)
    return true;
  else 
  {
    SYMBOL_TABLE symbolTable = scopeStack.top();
    scopeStack.pop();
    isFound = findEntryInAnyScope(the_name);
    scopeStack.push(symbolTable); // restore stack to original state
    return isFound;
  }
}

TYPE_INFO Parser::findInfoInAnyScope(const string the_name)
{
  TYPE_INFO tmp_info;
  tmp_info.start = UNDEFINED;
  tmp_info.end = UNDEFINED;
  tmp_info.type = UNDEFINED;      
  tmp_info.numParams = UNDEFINED; 
  tmp_info.returnType = UNDEFINED;
  tmp_info.isParam = UNDEFINED;  
  tmp_info.isArray = UNDEFINED;
  if (scopeStack.empty()) 
    return tmp_info;
  tmp_info = scopeStack.top().findEntry(the_name);
  if (tmp_info.type != UNDEFINED)
    return tmp_info;
  else 
  {
    SYMBOL_TABLE symbolTable = scopeStack.top();
    scopeStack.pop();
    tmp_info = findInfoInAnyScope(the_name);
    scopeStack.push(symbolTable); // restore stack to original state
    return tmp_info;
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

    void Parser::printAddition(const string item, const TYPE_INFO parts)
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
      exit(0);
    }


    void Parser::prog(Lexer &lex, vector<string> &currentToken) 
    {
      beginScope();
      printRule("N_PROG", 
        "N_PROGLBL T_IDENT T_SCOLON N_BLOCK T_DOT"); 
      progLbl(lex, currentToken);
      if (currentToken[0] == "T_IDENT") 
      {
        TYPE_INFO tmp;
        tmp.type = T_PROG;
        scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(currentToken[1], tmp));
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
      if(array_temp.isArray)
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
        exit(0);
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
        array_temp.isArray = true;
      }
      else 
      {
        printRule("N_TYPE", "N_SIMPLE");
    //simple type
        tmp_simple.type = simple(lex, currentToken);
        array_temp.isArray = false;
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
    if(array_temp.start > array_temp.end)
    {
      cout << "Line" << currentToken[2] <<": Start index must be less than or equal to end index of array" << endl;
    }

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
          TYPE_INFO tmp;
          tmp.type = T_PROC;
          bool inScope = findEntryInScope(currentToken[1]);
          scopeStack.top().addEntry(SYMBOL_TABLE_ENTRY(currentToken[1], tmp));
          printAddition(currentToken[1], T_PROC);
          if(inScope)
          {
        //already defined, bail out here
            cout << "Line " << currentToken[2] <<": Multiply defined identifier   "<<endl;
            exit(0);
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

void Parser::assignStmt(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO left_hand, right_hand;
  printRule("N_ASSIGN", "N_VARIABLE T_ASSIGN N_EXPR");
  left_hand = variable(lex, currentToken);
  if (currentToken[0] == "T_ASSIGN")
  {
    currentToken = lex.getToken();
    right_hand = expr(lex, currentToken);
    if (left_hand.type == T_PROC || left_hand.type == T_PROG)
    {
      cout << "Line" << currentToken[2] << ": Procedure/variable mismatch" << endl;
      exit(0);
    }
    if (right_hand.type != left_hand.type)
    {
      cout << "Line" << currentToken[2] << ": Expression must be of same type as variable" << endl;
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
    TYPE_INFO tmp_info;
    printRule("N_INPUTVAR", "N_VARIABLE");
    tmp_info = variable(lex, currentToken);
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
    TYPE_INFO tmp_info;
    printRule("N_OUTPUT", "N_EXPR");
    tmp_info = expr(lex, currentToken);
    if (tmp_info.type != T_INT && tmp_info.type != T_CHAR)
    {
      cout << "Line " << currentToken[2] << ": Output expression must be of type integer or char" << endl;
      exit(0);
    }
  }

void Parser::conditionStmt(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO tmp_info;
  printRule("N_CONDITION", "T_IF N_EXPR T_THEN N_STMT N_ELSEPART");
  if (currentToken[0] == "T_IF")
  {
    currentToken = lex.getToken();
    tmp_info = expr(lex, currentToken);
                                                 if(tmp_info.type != T_BOOL)
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
  TYPE_INFO tmp_info;
  printRule("N_WHILE", "T_WHILE N_EXPR T_DO N_STMT");
  if (currentToken[0] == "T_WHILE")
  {
    currentToken = lex.getToken();
    tmp_info = expr(lex, currentToken);
    if(tmp_info.type != T_BOOL)
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
    TYPE_INFO tmp_info1, tmp_info2;
    printRule("N_EXPR", "N_SIMPLEEXPR N_OPEXPR");
    tmp_info1 = simpleExpr(lex, currentToken);
    cout << "TYPE: " << tmp_info1.type << endl;
    tmp_info2 = opExpr(lex, currentToken);
    cout << "TYPE: " << tmp_info2.type << endl;
    if(tmp_info2.type != NOT_APPLICABLE)
    {   
      //we don't care about the actual type of either at this point
      tmp_info1.type = T_BOOL;
      return tmp_info1;
    }
    else if (tmp_info1.type != tmp_info2.type)
    {
      cout << "Line: " << currentToken[2] << ": Expressions must both be int, or both char, or both boolean" << endl;
      exit(0);
    }
    else
    {
      return  tmp_info1;
    }
  }

TYPE_INFO Parser::opExpr(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO tmp_info;
  if ((currentToken[0] == "T_LT") || 
    (currentToken[0] == "T_LE")   || 
    (currentToken[0] == "T_NE")   || 
    (currentToken[0] == "T_EQ")   || 
    (currentToken[0] == "T_GT")   || 
    (currentToken[0] == "T_GE"))
  { 
    printRule("N_OPEXPR", "N_RELOP N_SIMPLEEXPR");
    relOp(lex, currentToken);
    tmp_info = simpleExpr(lex, currentToken);
  }
  else
  {
    printRule("N_ADDOPLST", "epsilon");
    tmp_info.type = NOT_APPLICABLE;
  }
  return tmp_info;
}

TYPE_INFO Parser::simpleExpr(Lexer &lex, vector<string> &currentToken) 
{
  TYPE_INFO tmp_info;
  printRule("N_SIMPLEEXPR", "N_TERM N_ADDOPLST");
  tmp_info = term(lex, currentToken);
  addOpLst(lex, currentToken);
  return tmp_info;
}

  TYPE_INFO Parser::addOpLst(Lexer &lex, vector<string> &currentToken) 
  {
    TYPE_INFO tmp_info, tmp_info1, tmp_info2;
    if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS"))
    {
      printRule("N_ADDOPLST", "N_ADDOP N_TERM N_ADDOPLST");
      tmp_info1 = addOp(lex, currentToken);
      tmp_info = term(lex, currentToken);
      tmp_info2 = addOpLst(lex, currentToken);
      if(tmp_info1.type != T_INT || tmp_info2.type != T_INT)
      {
        cout << "Line " << currentToken[2] << ": Expression must be of type integer" << endl;
        exit(0);
      }
    } 
    else if (currentToken[0] == "T_OR")
    {
      printRule("N_ADDOPLST", "N_ADDOP N_TERM N_ADDOPLST");
      tmp_info1 = addOp(lex, currentToken);
      tmp_info = term(lex, currentToken);
      tmp_info2 = addOpLst(lex, currentToken);
      if(tmp_info1.type != T_BOOL || tmp_info2.type != T_BOOL)
      {
        cout << "Line " << currentToken[2] << ": Expression must be of type boolean" << endl;
        exit(0);
      }
    } 
    else 
    {
      printRule("N_ADDOPLST", "epsilon");
      tmp_info.type = NOT_APPLICABLE;
    }
    return tmp_info;
  }

  TYPE_INFO Parser::term(Lexer &lex, vector<string> &currentToken) 
  {
    TYPE_INFO tmp_info;
    printRule("N_TERM", "N_FACTOR N_MULTOPLST");
    tmp_info = factor(lex, currentToken);
    multOpLst(lex, currentToken);
    return tmp_info;
  }

  TYPE_INFO Parser::multOpLst(Lexer &lex, vector<string> &currentToken) 
  {
    TYPE_INFO tmp_info, tmp_info1, tmp_info2;
    if ((currentToken[0] == "T_MULT") || 
      (currentToken[0] == "T_DIV")) 
    {
      printRule("N_MULTOPLST", "N_MULTOP N_FACTOR N_MULTOPLST");
      tmp_info1 = multOp(lex, currentToken);
      tmp_info = factor(lex, currentToken);
      tmp_info2 = multOpLst(lex, currentToken);
      if(tmp_info1.type != T_INT || tmp_info2.type != T_INT)
      {
        cout << "Line " << currentToken[2] << ": Expression must be of type integer" << endl;
        exit(0);
      }
    }
    else if (currentToken[0] == "T_AND")
    {
      printRule("N_MULTOPLST", "N_MULTOP N_FACTOR N_MULTOPLST");
      tmp_info1 = multOp(lex, currentToken);
      tmp_info =  tmp_info = factor(lex, currentToken);
      tmp_info2 = multOpLst(lex, currentToken);
      if(tmp_info1.type != T_BOOL || tmp_info2.type != T_BOOL)
      {
        cout << "Line " << currentToken[2] << ": Expression must be of type boolean" << endl;
        exit(0);
      }
    }
    else 
    {
      tmp_info.type = NOT_APPLICABLE;
      printRule("N_MULTOPLST", "epsilon");
    }
    return tmp_info;
  }

TYPE_INFO Parser::factor(Lexer &lex, vector<string> &currentToken) 
{
  bool hasSign;
  TYPE_INFO tmp_info;
  if (currentToken[0] == "T_LPAREN")
  {
    printRule("N_FACTOR", "T_LPAREN N_EXPR T_RPAREN");
    currentToken = lex.getToken();
    tmp_info = expr(lex, currentToken);
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
      tmp_info.type = T_BOOL;
    }
    else
    {
      if ((currentToken[0] == "T_INTCONST") ||
         (currentToken[0] == "T_CHARCONST") ||
         (currentToken[0] == "T_TRUE") || 
         (currentToken[0] == "T_FALSE"))
      {
        printRule("N_FACTOR", "N_CONST");
        tmp_info = constant(lex, currentToken);
      }
      else
      {
        printRule("N_FACTOR", "N_SIGN N_VARIABLE");
        hasSign = sign(lex, currentToken);
        tmp_info = variable(lex, currentToken);
        if(hasSign && tmp_info.type != T_INT)
        {
          cout << "Line" << currentToken[2] << ": Expression must be of type integer" << endl;
          exit(0);
        }
      }
    }
  }
  return tmp_info;
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
    TYPE_INFO tmp_info;
    if ((currentToken[0] == "T_PLUS") || 
      (currentToken[0] == "T_MINUS")  )
    { 
      tmp_info.type = T_INT;
      printRule("N_ADDOP", currentToken[0]);  
      currentToken = lex.getToken();
    }
    else if (currentToken[0] == "T_OR")
    { 
      tmp_info.type = T_BOOL;
      printRule("N_ADDOP", currentToken[0]);  
      currentToken = lex.getToken();
    }
    else syntaxError(currentToken);
    return tmp_info;
  }

  TYPE_INFO Parser::multOp(Lexer &lex, vector<string> &currentToken) 
  {
    TYPE_INFO tmp_info;
    if ((currentToken[0] == "T_MULT") || 
      (currentToken[0] == "T_DIV"))
    {
      tmp_info.type = T_INT;
      printRule("N_MULTOP", currentToken[0]);  
      currentToken = lex.getToken();
    }
    else if (currentToken[0] == "T_AND")
    {
      tmp_info.type = T_BOOL;
      printRule("N_MULTOP", currentToken[0]);  
      currentToken = lex.getToken();
    }
    else syntaxError(currentToken);
    return tmp_info;
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
    bool isIndexed;
    TYPE_INFO tmp_info;
    printRule("N_VARIABLE", "T_IDENT N_IDXVAR");
    if (currentToken[0] == "T_IDENT")
    {
      
      if(!findEntryInAnyScope(currentToken[1]))
      {
        cout << "Line " << currentToken[2] <<": Undefined identifier"<<endl;
        exit(0);
      }
      currentToken = lex.getToken();
      isIndexed = idxVar(lex, currentToken);
      tmp_info = findInfoInAnyScope(currentToken[1]);
      cout << "variableType" << tmp_info.type << endl;
      if(!tmp_info.isArray && isIndexed)
      {
        cout << "Line " << currentToken[2] << ": Indexed variable must be of array type" << endl;
        exit(0);
      }
    }
    else syntaxError(currentToken);
    return tmp_info;
  }

  bool Parser::idxVar(Lexer &lex, vector<string> &currentToken) 
  {
    TYPE_INFO tmp_info;
    if (currentToken[0] == "T_LBRACK")
    {
      printRule("N_IDXVAR", "T_LBRACK N_EXPR T_RBRACK");
      currentToken = lex.getToken();
      tmp_info = expr(lex, currentToken);
      if (tmp_info.type == UNDEFINED || tmp_info.type == NOT_APPLICABLE)
      {
        //Crash out here: Array must be indexed
        cout << "Line "<< currentToken[2] <<": Array variable must be indexed"<< endl;
        exit(0);
      }
      else if (tmp_info.type == T_PROC || tmp_info.type == T_PROG)
      {
        //Crash out here: index cannot be proc
        cout << "Line "<< currentToken[2] <<": Procedure/variable mismatch"<< endl;
        exit(0);
      }
      else if (tmp_info.type != T_INT)
      {
        //Crash out here: index must be int
        cout << "Line "<< currentToken[2] <<": Index expression must be of type integer"<< endl;
        exit(0);
      }
      if (currentToken[0] == "T_RBRACK")
        currentToken = lex.getToken();
      else syntaxError(currentToken);
    }
    else
    {
      printRule("N_IDXVAR", "epsilon");
      return false;
    }
    return true;
  }

  TYPE_INFO Parser::constant(Lexer &lex, vector<string> &currentToken) 
  {
    TYPE_INFO tmp_info;
    if ((currentToken[0] == "T_TRUE") ||
      (currentToken[0] == "T_FALSE"))
    {
      tmp_info.type = T_BOOL;
      printRule("N_CONST", "N_BOOLCONST");
      boolConst(lex, currentToken);
    }
    else if (currentToken[0] == "T_INTCONST")
    {
      tmp_info.type = T_INT;
      printRule("N_CONST", currentToken[0]);
      currentToken = lex.getToken();
    }  
    else if (currentToken[0] == "T_CHARCONST")
    {
      tmp_info.type = T_CHAR;
      printRule("N_CONST", currentToken[0]);
      currentToken = lex.getToken();
   }
   else syntaxError(currentToken);
   return tmp_info;
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











