%{
#include<stdio.h>
#include<stdlib.h>
int yylex();
void yyerror(const char *s);
%}

%union {int num;}
%start line
%token <num> DIGIT
%type <num> line expr term

%%
line : expr ';'       {printf("done %d\n",$1);}
     ;

expr : term
     | expr '+' term  {printf("add %d %d\n",$1,$3); $$=$1+$3;}
     | expr '-' term  {printf("sub %d %d\n",$1,$3); $$=$1-$3;}
     ;

term : DIGIT          {printf("digit %d\n",$1); $$=$1;}
     ;
%%

int main()
{
    yyparse();
    return 1;
}

void yyerror(const char *s)
{
    printf("error\n");
}
