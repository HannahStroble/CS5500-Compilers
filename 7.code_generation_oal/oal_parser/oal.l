/*
      oal.l

 	Specifications of tokens for the OAL language.
 	CS 5500
 */

/*
 *	Constant definitions
 */
%{

%}

/*
 *	Definitions of regular expressions
*/

NEWLINE         \n
WSPACE		[ \t\v\r]+

NEG_INTCONST	"-"[0-9]+
POS_INTCONST	[0-9]+

LABEL			"L."[0-9]+

/*
 *	Tokens
*/
%%

"#"			{
				/* if comment, skip until eoln. */
				ignoreComment();
			}
","			{
				printTokenInfo("T_COMMA", yytext);
				return T_COMMA;
			}
":"			{
				printTokenInfo("T_COLON", yytext);
				return T_COLON;
			}
"jp"			{
				printTokenInfo("T_JP", yytext);
				return T_JP;
			}
"jf"			{
				printTokenInfo("T_JF", yytext);
				return T_JF;
			}
"jt"			{
				printTokenInfo("T_JT", yytext);
				return T_JT;
			}
"js"			{
				printTokenInfo("T_JS", yytext);
				return T_JS;
			}
"ji"			{
				printTokenInfo("T_JI", yytext);
				return T_JI;
			}
"bss"			{
				printTokenInfo("T_BSS", yytext);
				return T_BSS;
			}
"asp"			{
				printTokenInfo("T_ASP", yytext);
				return T_ASP;
			}
"push"		{
				printTokenInfo("T_PUSH", yytext);
				return T_PUSH;
			}
"pop"			{
				printTokenInfo("T_POP", yytext);
				return T_POP;
			}
"save" 		{
				printTokenInfo("T_SAVE", yytext);
				return T_SAVE;
			}
"lc"			{
				printTokenInfo("T_LC", yytext);
				return T_LC;
			}
"lv"			{
				printTokenInfo("T_LV", yytext);
				return T_LV;
			}
"la"			{
				printTokenInfo("T_LA", yytext);
				return T_LA;
			}
"deref"		{
				printTokenInfo("T_DEREF", yytext);
				return T_DEREF;
			}
"st"			{
				printTokenInfo("T_ST", yytext);
				return T_ST;
			}
"add"			{
				printTokenInfo("T_ADD", yytext);
				return T_ADD;
			}
"sub"			{
				printTokenInfo("T_SUB", yytext);
				return T_SUB;
			}
"mult"		{
				printTokenInfo("T_MULT", yytext);
				return T_MULT;
			}
"div"			{
				printTokenInfo("T_DIV", yytext);
				return T_DIV;
			}
"mod"			{
				printTokenInfo("T_MOD", yytext);
				return T_MOD;
			}
"and"			{
				printTokenInfo("T_AND", yytext);
				return T_AND;
			}
"or"			{
				printTokenInfo("T_OR", yytext);
				return T_OR;
			}
".eq."		{
				printTokenInfo("T_EQ", yytext);
				return T_EQ;
			}
".ne."		{
				printTokenInfo("T_NE", yytext);
				return T_NE;
			}
".lt."		{
				printTokenInfo("T_LT", yytext);
				return T_LT;
			}
".le."		{
				printTokenInfo("T_LE", yytext);
				return T_LE;
			}
".gt."		{
				printTokenInfo("T_GT", yytext);
				return T_GT;
			}
".ge."		{
				printTokenInfo("T_GE", yytext);
				return T_GE;
			}
"neg"			{
				printTokenInfo("T_NEG", yytext);
				return T_NEG;
			}
"not"	 		{
				printTokenInfo("T_NOT", yytext);
				return T_NOT;
			}
"cread"		{
				printTokenInfo("T_CREAD", yytext);
				return T_CREAD;
			}
"iread"		{
				printTokenInfo("T_IREAD", yytext);
				return T_IREAD;
			}
"cwrite"		{
				printTokenInfo("T_CWRITE", yytext);
				return T_CWRITE;
			}
"iwrite"		{
				printTokenInfo("T_IWRITE", yytext);
				return T_IWRITE;
			}
"init"		{
				printTokenInfo("T_INIT", yytext);
				return T_INIT;
			}
"halt"		{
				printTokenInfo("T_HALT", yytext);
				return T_HALT;
			}
"end"			{
				printTokenInfo("T_END", yytext);
				return T_END;
			}
{NEG_INTCONST}	{
				printTokenInfo("T_NCONST", yytext);
                      yylval.num = atoi(yytext);
				return T_NCONST;
			}
{POS_INTCONST}	{
				printTokenInfo("T_PCONST", yytext);
                      yylval.num = atoi(yytext);
				return T_PCONST;
			}
{LABEL}		{
				sprintf(yylval.cstring, "%s", yytext);
				if ((strcmp(entryLabel, "0") != 0) && 
                          !strcmp(yytext, entryLabel))
				{
				  printTokenInfo("T_ENTRY", yytext);
				  return T_ENTRY;
				}
				else
				{
				  printTokenInfo("T_LABEL", yytext);
				  return T_LABEL;
				}
			}
{NEWLINE}   	{
                		++lineNum;
            	}
{WSPACE}		{ }
.			{
				printTokenInfo("UNKNOWN", yytext);
				return T_UNKNOWN;
			}

%%




