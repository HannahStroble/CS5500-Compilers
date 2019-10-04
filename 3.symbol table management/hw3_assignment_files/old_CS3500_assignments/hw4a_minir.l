/* minir.l
    
*/

%{

%}

WSPACE [ \t\v\r]+
NEWLINE \n

DIGIT [0-9]
LETTER [a-zA-Z]

IDENT ({LETTER}|\_)+({LETTER}|{DIGIT}|\_)*

FLOATCONST (\+|-)?(({DIGIT})*\.({DIGIT})+)
INTCONST (\+|-)?{DIGIT}+

STRCONST (\")[^\"\t\v\r\n]+(\")
COMMENT #+.*


%%

"(" {
    printTokenInfo("LPAREN", yytext);
    return T_LPAREN;
}

")" {
    printTokenInfo("RPAREN", yytext);
    return T_RPAREN;
}

"{" {
    printTokenInfo("LBRACE", yytext);
    return T_LBRACE;
}

"}" {
    printTokenInfo("RBRACE", yytext);
    return T_RBRACE;
}

"[" {
    printTokenInfo("LBRACKET", yytext);
    return T_LBRACKET;
}

"]" {
    printTokenInfo("RBRACKET", yytext);
    return T_RBRACKET;
}

"+" {
    printTokenInfo("ADD", yytext);
    return T_ADD;
}

"-" {
    printTokenInfo("SUB", yytext);
    return T_SUB;
}

"*" {
    printTokenInfo("MULT", yytext);
    return T_MULT;
}

"/" {
    printTokenInfo("DIV", yytext);
    return T_DIV;
}

"%%" {
    printTokenInfo("MOD", yytext);
    return T_MOD;
}

"^" {
    printTokenInfo("POWER", yytext);
    return T_POW;
}

"<" {
    printTokenInfo("LT", yytext);
    return T_LT;
}

"<=" {
    printTokenInfo("LE", yytext);
    return T_LE;
}

">" {
    printTokenInfo("GT", yytext);
    return T_GT;
}

">=" {
    printTokenInfo("GE", yytext);
    return T_GE;
}

"==" {
    printTokenInfo("EQ", yytext);
    return T_EQ;
}

"!=" {
    printTokenInfo("NE", yytext);
    return T_NE;
}

"!" {
    printTokenInfo("NOT", yytext);
    return T_NOT;
}

"&" {
    printTokenInfo("AND", yytext);
    return T_AND;
}

"|" {
    printTokenInfo("OR", yytext);
    return T_OR;
}

"=" {
    printTokenInfo("ASSIGN", yytext);
    return T_ASSIGN;
}

";" {
    printTokenInfo("SEMICOLON", yytext);
    return T_SEMICOLON;
}

"," {
    printTokenInfo("COMMA", yytext);
    return T_COMMA;
}

"TRUE" {
    printTokenInfo("TRUE", yytext);
    return T_TRUE;
}

"FALSE" {
    printTokenInfo("FALSE", yytext);
    return T_FALSE;
}

"if" {
    printTokenInfo("IF", yytext);
    return T_IF;
}

"else" {
    printTokenInfo("ELSE", yytext);
    return T_ELSE;
}

"while" {
    printTokenInfo("WHILE", yytext);
    return T_WHILE;
}

"function" {
    printTokenInfo("FUNCTION", yytext);
    return T_FUNCTION;
}

"for" {
    printTokenInfo("FOR", yytext);
    return T_FOR;
}

"in" {
    printTokenInfo("IN", yytext);
    return T_IN;
}

"quit" {
    printTokenInfo("QUIT", yytext);
    return T_QUIT;
}

"print" {
    printTokenInfo("PRINT", yytext);
    return T_PRINT;
}

"cat" {
    printTokenInfo("CAT", yytext);
    return T_CAT;
}

"read" {
    printTokenInfo("READ", yytext);
    return T_READ;
}

"list" {
    printTokenInfo("LIST", yytext);
    return T_LIST;
}

{STRCONST} {
    printTokenInfo("STRCONST", yytext);
    return T_STRCONST;
}

{INTCONST} {
    printTokenInfo("INTCONST", yytext);
    return T_INTCONST;
}

{FLOATCONST} {
    printTokenInfo("FLOATCONST", yytext);
    return T_FLOATCONST;
}

{IDENT} {
    printTokenInfo("IDENT", yytext);
    yylval.text = strdup(yytext);
    return T_IDENT;
}

{NEWLINE} {
    line_num++;
}

{WSPACE} {}

{COMMENT} {}

. {
    printTokenInfo("UNKNOWN", yytext);
    return T_UNKNOWN;
}

%%