/*
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

%{
/* 
 * SECTION 1: C Declarations 
 * Include standard C libraries required for strings, IO, and memory[cite: 46].
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* 
 * Enable Flex's built-in line number tracking[cite: 47]. 
 * Yylineno automatically increments on newlines and is globally accessible.
 */
%option yylineno

/* 
 * Symbol Table Definition[cite: 46, 47]
 * We use a fixed-size array of structs to keep track of every unique identifier[cite: 47].
 */
#define MAX_SYMBOLS 1000

typedef struct {
    char name[128];
    int first_line;
    int count;
} Symbol;

Symbol sym_table[MAX_SYMBOLS];
int sym_count = 0;

/* 
 * Symbol Table Insert/Update Logic[cite: 47]
 * A linear search is performed on each IDENTIFIER match[cite: 47].
 * If found, increment the count. If not, add a new entry.
 */
void add_symbol(const char *lex, int line) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(sym_table[i].name, lex) == 0) {
            sym_table[i].count++;
            return;
        }
    }
    if (sym_count < MAX_SYMBOLS) {
        strncpy(sym_table[sym_count].name, lex, 127);
        sym_table[sym_count].name[127] = '\0';
        sym_table[sym_count].first_line = line;
        sym_table[sym_count].count = 1;
        sym_count++;
    }
}
%}

%%
    /* --- Whitespace --- */
    /* Ignore spaces, tabs, carriage returns, and explicitly newlines (yylineno handles counting)[cite: 47] */
[ \t\r\n]+               { /* Ignore */ }


    /* --- Keywords --- */
    /* MUST be listed before the IDENTIFIER rule so they are not incorrectly classified[cite: 47, 48]. */
"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void" {
    printf("%d: %s -> KEYWORD\n", yylineno, yytext);
}


    /* --- Identifiers --- */
    /* Matches standard C identifiers and logs them in the symbol table[cite: 47]. */
[a-zA-Z_][a-zA-Z0-9_]*   {
    printf("%d: %s -> IDENTIFIER\n", yylineno, yytext);
    add_symbol(yytext, yylineno);
}


    /* --- Numbers --- */
    /* Well-formed numbers: Integers or simple decimals[cite: 47] */
[0-9]+(\.[0-9]+)?        {
    printf("%d: %s -> NUMBER\n", yylineno, yytext);
}
    
    /* Malformed numbers: Follows immediately after the valid rule to catch ties[cite: 48].
       Matches strings starting with digits but containing invalid characters (e.g., 3.14.15, 12abc).
       This prevents the lexer from falling through and throwing confusing single-character errors[cite: 48]. */
[0-9]+[a-zA-Z0-9_.]*     {
    printf("Lexical Error at line %d: malformed number '%s'\n", yylineno, yytext);
}


    /* --- Strings --- */
    /* Well-formed string literal (does not allow newlines inside quotes)[cite: 47, 48] */
\"[^\"\n]*\"             {
    printf("%d: %s -> STRING\n", yylineno, yytext);
}

    /* Unterminated string literal: Hit a newline or EOF without a closing quote[cite: 47, 48] */
\"[^\"\n]*               {
    printf("Lexical Error at line %d: unterminated string literal\n", yylineno);
}


    /* --- Operators --- */
    /* Two-character operators MUST be listed before single-character operators to ensure longest match[cite: 47, 48]. */
"=="|"<="|">="|"!="      { printf("%d: %s -> OPERATOR\n", yylineno, yytext); }
"+"|"-"|"*"|"/"|"="|"<"|">" { printf("%d: %s -> OPERATOR\n", yylineno, yytext); }


    /* --- Punctuation --- */
";"|","|"("|")"|"{"|"}"  { printf("%d: %s -> PUNCTUATION\n", yylineno, yytext); }


    /* --- End of File --- */
    /* When input is exhausted, print the complete symbol table exactly once before exiting[cite: 47, 48]. */
<<EOF>>                  {
    printf("\nSymbol Table:\n");
    for (int i = 0; i < sym_count; i++) {
        printf("%s\tfirst line %d\toccurrences %d\n", sym_table[i].name, sym_table[i].first_line, sym_table[i].count);
    }
    return 0; // Terminate scanner
}


    /* --- Catch-all / Error Recovery --- */
    /* Lexical Error Recovery: Matches any character that cannot begin a valid token (e.g., @, $, #)[cite: 47].
       Because the rule is empty except for the print statement, scanning simply resumes with the very next character[cite: 47].
       MUST be placed last in the rules list[cite: 48]. */
.                        {
    printf("Lexical Error at line %d: unexpected character '%s'\n", yylineno, yytext);
}

%%

int main(int argc, char **argv) {
    // Start scanning from standard input[cite: 46]
    yylex();
    return 0;
}