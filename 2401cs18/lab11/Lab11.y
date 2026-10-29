%{
/*
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int yylex(void);
void yyerror(const char *s);

int yylineno = 1;
int error_flag = 0;
%}

%token ID NUM
%left '+' '-'
%left '*' '/'

%%

program:
    statements
    | /* empty */
    ;

statements:
    statements statement
    | statement
    ;

statement:
    ID '=' expr ';'
    ;

expr:
    expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'
    | ID
    | NUM
    ;

%%

/* Custom Lexical Analyzer in C */
int yylex(void) {
    int c;

    /* Skip whitespaces and track newlines for error reporting */
    while ((c = getchar()) == ' ' || c == '\t' || c == '\n' || c == '\r') {
        if (c == '\n') {
            /* Peek ahead to avoid incrementing line number on trailing newlines before EOF */
            int next_c = getchar();
            if (next_c == EOF) {
                return 0;
            }
            ungetc(next_c, stdin);
            yylineno++;
        }
    }

    if (c == EOF) {
        return 0;
    }

    /* Recognize integer numbers */
    if (isdigit(c)) {
        while (isdigit(c = getchar())) {
            /* Consume digits */
        }
        ungetc(c, stdin); 
        return NUM;
    }

    /* Recognize identifiers */
    if (isalpha(c)) {
        while (isalnum(c = getchar())) {
            /* Consume alphanumeric characters */
        }
        ungetc(c, stdin);
        return ID;
    }

    /* Return other characters directly as tokens */
    return c;
}

/* Error reporting function */
void yyerror(const char *s) {
    printf("Syntax error at line %d\n", yylineno);
    error_flag = 1;
}

int main(void) {
    /* yyparse() returns 0 on successful parsing of the entire input */
    if (yyparse() == 0 && error_flag == 0) {
        printf("Input is syntactically valid.\n");
    }
    return 0;
}