/*   hw5.l
 
 	Specifications of tokens for HW #5, to be
      used with bison file hw5.y

 	CS 5500, Fall 2019.
     
 */

/*
 *	Constant definitions
 */
%{

%}

/*
 *	Definitions of regular expressions
*/

NEWLINE         [\n]
WSPACE		[ \t\r]+

NEG_INTCONST	"-"[0-9]+
POS_INTCONST	("+")?[0-9]+
INTCONST        {NEG_INTCONST}|{POS_INTCONST}

IDENT			[a-z]|[A-Z]
 
/*
 *	Tokens
*/
%%

"("			{
				printTokenInfo("T_LPAREN", yytext);
				return T_LPAREN;
			}
")"			{
				printTokenInfo("T_RPAREN", yytext);
				return T_RPAREN;
			}
"["			{
				printTokenInfo("T_LBRACK", yytext);
				return T_LBRACK;
			}
"]"			{
				printTokenInfo("T_RBRACK", yytext);
				return T_RBRACK;
			}
";"			{
				printTokenInfo("T_SEMICOL", yytext);
				return T_SEMICOL;
			}
"+"			{
				printTokenInfo("T_PLUS", yytext);
				return T_PLUS;
			}
"="			{
				printTokenInfo("T_ASSIGN", yytext);
				return T_ASSIGN;
			}
">"			{
				printTokenInfo("T_GT", yytext);
				return T_GT;
			}
"<"			{
				printTokenInfo("T_LT", yytext);
				return T_LT;
			}
"!="			{
				printTokenInfo("T_NE", yytext);
				return T_NE;
			}
">="			{
				printTokenInfo("T_GE", yytext);
				return T_GE;
			}
"<="			{
				printTokenInfo("T_LE", yytext);
				return T_LE;
			}
"=="			{
				printTokenInfo("T_EQ", yytext);
				return T_EQ;
			}
"var"			{
				printTokenInfo("T_VAR", yytext);
				return T_VAR;
			}
"{"			{
				printTokenInfo("T_LCURLY", yytext);
                      return T_LCURLY;
			}
"}"			{
				printTokenInfo("T_RCURLY", yytext);
                      return T_RCURLY;
			}
"if"			{
				printTokenInfo("T_IF", yytext);
				return T_IF;
			}
"then"		{
				printTokenInfo("T_THEN", yytext);
				return T_THEN;
			}
"else"		{
				printTokenInfo("T_ELSE", yytext);
				return T_ELSE;
			}
"while"		{
				printTokenInfo("T_WHILE", yytext);
				return T_WHILE;
			}
"true"		{
				printTokenInfo("T_TRUE", yytext);
				return T_TRUE;
			}
"false"		{
				printTokenInfo("T_FALSE", yytext);
				return T_FALSE;
		     }
{INTCONST}	     {     printTokenInfo("T_INTCONST", yytext);
                      yylval.num = atoi(yytext);
				return T_INTCONST;
			}
{IDENT}		{
                      printTokenInfo("T_IDENT", yytext);
                      yylval.ch = yytext[0];
				return T_IDENT;
			}
{NEWLINE}   	{
                	lineNum++;
            	}
{WSPACE}		{ }
.			{
				printTokenInfo("T_UNKNOWN", yytext);
				return T_UNKNOWN;
			}

%%





