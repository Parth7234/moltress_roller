/* Compile: bison -d com.y && gcc com.tab.c -o com */

%{
/*
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

/* 
 * SECTION 1: C Declarations 
 * Code enclosed in %{ and %} is copied verbatim into the generated Lab11.tab.c file. 
 * This section includes standard libraries and global variables needed by the parser.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Function prototypes for the lexer and error handler required by Yacc
int yylex(void);
void yyerror(const char *s);

// Global state trackers
int yylineno = 1;     // Tracks the current line number for accurate syntax error reporting[cite: 40]
int error_flag = 0;   // Flag to prevent printing "syntactically valid" if an error occurred[cite: 40]
%}

/* 
 * SECTION 2: Yacc Declarations
 * Defines the terminal tokens and their precedence/associativity rules.
 */
%token ID NUM

/* 
 * Operator Precedence and Associativity[cite: 38]
 * Yacc resolves parsing conflicts based on the order of these declarations.
 * Rules declared lower in this list have HIGHER precedence.
 * Both sets are left-associative (%left), meaning 'a + b + c' parses as '(a + b) + c'[cite: 38].
 */
%left '+' '-'   // Addition and subtraction have lower precedence[cite: 38]
%left '*' '/'   // Multiplication and division have higher precedence[cite: 38]

%%

/* 
 * SECTION 3: Grammar Rules
 * This defines the Context-Free Grammar (CFG) the parser will accept. 
 * The format is: non_terminal: productions ;
 */

program:
    statements
    | /* empty */   // Allows the parser to accept an empty file without throwing an error
    ;

statements:
    statements statement // Recursive rule: allows multiple statements one after another[cite: 39]
    | statement          // Base case: a single statement
    ;

/* 
 * Assignment Statement[cite: 39]
 * Enforces the structure: an identifier, an equals sign, an expression, and a mandatory semicolon[cite: 39, 40].
 */
statement:
    ID '=' expr ';'
    ;

/* 
 * Arithmetic Expressions[cite: 38]
 * Defines all valid mathematical operations. Yacc automatically applies the precedence 
 * rules defined above to resolve ambiguities (like in a + b * c)[cite: 38].
 */
expr:
    expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | '(' expr ')'   // Parentheses naturally group expressions, overriding default precedence[cite: 38]
    | ID             // Base case: Variable identifier (e.g., 'a', 'result')
    | NUM            // Base case: Integer number (e.g., '10')
    ;

%%

/* 
 * SECTION 4: User Code (Custom Lexer and Main) 
 * Implements tokenization directly in C instead of using Lex/Flex[cite: 40].
 */

/* Custom Lexical Analyzer in C */
int yylex(void) {
    int c;

    /* 
     * Skip whitespaces and track newlines for error reporting[cite: 40].
     * The loop consumes spaces, tabs, and carriage returns.
     */
    while ((c = getchar()) == ' ' || c == '\t' || c == '\n' || c == '\r') {
        if (c == '\n') {
            /* Peek ahead to avoid incorrectly incrementing the line number on trailing newlines before EOF */
            int next_c = getchar();
            if (next_c == EOF) {
                return 0; // End of file reached
            }
            ungetc(next_c, stdin); // Put the character back into the stream if it's not EOF
            yylineno++;
        }
    }

    if (c == EOF) {
        return 0; // Tell Yacc there are no more tokens to parse
    }

    /* 
     * Recognize integer numbers[cite: 38, 40]
     * If a digit is found, consume all subsequent digits to form a single NUM token.
     */
    if (isdigit(c)) {
        while (isdigit(c = getchar())) {
            /* Consume digits */
        }
        ungetc(c, stdin); // The loop breaks on a non-digit. Put that non-digit back for the next yylex() call.
        return NUM;
    }

    /* 
     * Recognize identifiers (variables)[cite: 38, 40]
     * If a letter is found, consume all subsequent alphanumeric characters to form an ID token.
     */
    if (isalpha(c)) {
        while (isalnum(c = getchar())) {
            /* Consume alphanumeric characters */
        }
        ungetc(c, stdin); // Put the non-alphanumeric character back into the stream
        return ID;
    }

    /* 
     * Return other characters directly as tokens[cite: 40]
     * This handles single-character operators and symbols like '+', '-', '=', '(', ')', and ';'
     */
    return c;
}

/* 
 * Error reporting function 
 * Called automatically by Yacc if the token stream violates the grammar rules (e.g., missing semicolons)[cite: 40].
 */
void yyerror(const char *s) {
    printf("Syntax error at line %d\n", yylineno); // Report exact line of failure[cite: 40]
    error_flag = 1; // Mark that an error occurred so we don't print a success message[cite: 40]
}

int main(void) {
    /* 
     * yyparse() initiates the parsing process. It returns 0 if the entire input matches the grammar.
     * The error_flag ensures that programs with syntax errors are never reported as successfully parsed[cite: 40].
     */
    if (yyparse() == 0 && error_flag == 0) {
        printf("Input is syntactically valid.\n");
    }
    return 0;
}