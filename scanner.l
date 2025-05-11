%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "ast.h"

extern int yylineno;
extern char* yytext;

void yyerror(const char *s);

%}

%option yylineno
%option noyywrap nounput noinput

DIGIT   [0-9]
INT     -?{DIGIT}+
EXP     [eE][+-]?{DIGIT}+
FLOAT   -?{DIGIT}+"."{DIGIT}*{EXP}?|{DIGIT}*"."{DIGIT}+{EXP}?|{DIGIT}+{EXP}

%%

[ \t]   ; // Ignore whitespace
\n      { yylineno++; }

\"([^"\\]|\\.)*\" {
    yylval.str = strndup(yytext+1, yyleng-2);
    return STRING;
}

{INT}   { yylval.num = atoi(yytext); return NUMBER; }
{FLOAT} { yylval.num = atof(yytext); return NUMBER; }
"true"  { yylval.boolean = 1; return BOOLEAN; }
"false" { yylval.boolean = 0; return BOOLEAN; }
"null"  { return NULL_TOKEN; }

"{"     { return '{'; }
"}"     { return '}'; }
"["     { return '['; }
"]"     { return ']'; }
":"     { return ':'; }
","     { return ','; }

.       { yyerror("Invalid character"); }

%%
