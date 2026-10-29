%{
/* Name: Parth Singla */
/* Roll Number: 2401CS18 */
#include <stdio.h>
#include <stdlib.h>
%}

/* exclusive start condition for multi-line comments */
%x COMMENT

%%

    /* Comment Skipping Rules */
"//"([^\n])*             { /* discard single-line comments */ }

"/*"                     { BEGIN(COMMENT); /* Enter COMMENT state on multi-line comment start */ }
<COMMENT>"*/"            { BEGIN(INITIAL); /* Return to normal state when comment ends */ }
<COMMENT>\n              { /* Consume newlines inside comments */ }
<COMMENT>.               { /* Consume any other character inside comments */ }
<COMMENT><<EOF>>         { printf("Error: unterminated comment\n"); return 0; /* Handle missing closing tag */ }

    /* Whitespace */
[ \t\n\r]+               { /* Ignore whitespace */ }

    /* Token Classification Rules */
    /* Keywords must be listed before identifiers */
"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void" { 
    printf("%s -> KEYWORD\n", yytext); 
}

    /* Identifiers */
[a-zA-Z][a-zA-Z0-9]*     { printf("%s -> IDENTIFIER\n", yytext); }

    /* Numbers (Integers and simple decimals) */
[0-9]+(\.[0-9]+)?        { printf("%s -> NUMBER\n", yytext); }

    /* Operators (Multi-character patterns must precede single-character ones) */
"=="|"<="|">="|"!="      { printf("%s -> OPERATOR\n", yytext); }
"+"|"-"|"*"|"/"|"="|"<"|">" { printf("%s -> OPERATOR\n", yytext); }

    /* Punctuation */
";"|","|"("|")"|"{"|"}"  { printf("%s -> PUNCTUATION\n", yytext); }

    /* Catch-all for unrecognized characters */
.                        { }

%%

int main(int argc, char **argv) {
    yylex();
    return 0;
}