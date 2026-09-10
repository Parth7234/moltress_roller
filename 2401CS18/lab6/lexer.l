%{
/*
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

#include <stdio.h>
#include <stdlib.h>

/* Statistics counters */
int lines = 1;
int single_comments = 0;
int multi_comments = 0;
int strings = 0;
int chars = 0;
int preproc = 0;
int identifiers = 0;

/* Flag for lexical errors (unterminated strings/comments/chars) */
int has_error = 0;
%}

/* Start condition for multi-line comments */
%x MULTI_COMMENT

%%

    /* 1. Multi-line comments */
"/*"                    { BEGIN(MULTI_COMMENT); }
<MULTI_COMMENT>"*/"     { multi_comments++; BEGIN(INITIAL); }
<MULTI_COMMENT>\n       { lines++; }
<MULTI_COMMENT>.        { /* Ignore characters inside multi-line comments */ }
<MULTI_COMMENT><<EOF>>  { has_error = 1; return 0; /* Unterminated multi-line comment error */ }

    /* 2. Single-line comments */
"//".*                  { single_comments++; }

    /* 3. Preprocessor directives */
^[ \t]*#.*              { preproc++; printf("%-18s %s\n", "PREPROCESSOR", yytext); }

    /* 4. String literals */
\"([^\\\n\"]|\\.)*\"    { strings++; printf("%-18s %s\n", "STRING LITERAL", yytext); }
\"([^\\\n\"]|\\.)*      { has_error = 1; return 0; /* Unterminated string literal error */ }

    /* 5. Character literals */
\'([^\\\n\']|\\.)*\'    { chars++; printf("%-18s %s\n", "CHAR LITERAL", yytext); }
\'([^\\\n\']|\\.)*      { has_error = 1; return 0; /* Unterminated character literal error */ }

"int"|"printf"          { /* Ignore keywords */ }

    /* 6. Identifiers (Keep this below the keyword rule) */
[a-zA-Z_][a-zA-Z0-9_]*  { identifiers++; printf("%-18s %s\n", "IDENTIFIER", yytext); }

    /* 7. Line tracking and ignoring other characters */
\n                      { lines++; }
[ \t\r]+                { /* Ignore whitespace */ }
.                       { /* Ignore all other ordinary source characters */ }

%%

int main() {
    yylex();

    printf("\nSource Code Statistics\n\n");
    printf("Lines\t\t\t: %d\n", lines);
    printf("Comments\t\t: %d\n", single_comments + multi_comments);
    printf("String literals\t\t: %d\n", strings);
    printf("Character literals\t: %d\n", chars);
    printf("Preprocessor\t\t: %d\n", preproc);
    printf("Identifiers\t\t: %d\n\n", identifiers);

    /* Final Verdict */
    if (has_error) {
        printf("REJECT\n");
    } else {
        printf("ACCEPT\n");
    }

    return 0;
}

int yywrap() {
    return 1;
}