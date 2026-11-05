%{
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
void yyerror(const char *s);
int yylex();
%}
%%
[0-9]+      {yylval.num = atoi(yytext); return DIGIT;}
[-+;]       {return yytext[0];}
.           {printf("unexpected character");}

%%
int yywrap (void) {return 1;}
