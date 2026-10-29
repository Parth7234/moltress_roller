/* Compile: flex com.l && gcc lex.yy.c -o com -lfl */
/* Run with the sample input: ./com < input.c */

%{
/*
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

#include <stdio.h>
#include <stdlib.h>

/* 
 * Statistics counters[cite: 14]
 * These global variables track the occurrences of different lexical constructs 
 * so we can print the required summary statistics at the end[cite: 17, 18].
 */
int lines = 1; // Starts at 1 to count the first line
int single_comments = 0;
int multi_comments = 0;
int strings = 0;
int chars = 0;
int preproc = 0;
int identifiers = 0;

/* Flag for lexical errors (unterminated strings/comments/chars)[cite: 14, 18] */
int has_error = 0;
%}

/* 
 * Start condition for multi-line comments[cite: 18]
 * %x defines an EXCLUSIVE state. When Flex enters this state, it ignores all 
 * other rules (like identifiers or strings) and only matches rules prefixed 
 * with <MULTI_COMMENT>. This prevents symbols inside comments from being parsed as code.
 */
%x MULTI_COMMENT

%%

    /* --- 1. Multi-line comments[cite: 15] --- */
    /* When "/*" is seen, switch the lexer into the MULTI_COMMENT state. */
"/*"                    { BEGIN(MULTI_COMMENT); }

    /* When the multi-line comment terminator is seen, increment the counter and return to the normal state[cite: 18]. */
<MULTI_COMMENT>"*/"     { multi_comments++; BEGIN(INITIAL); }

    /* Track line numbers accurately even when inside a massive block comment[cite: 18]. */
<MULTI_COMMENT>\n       { lines++; }

    /* Consume and ignore any other character inside the comment. */
<MULTI_COMMENT>.        { /* Ignore characters inside multi-line comments */ }

    /* If we hit the End of File (<<EOF>>) while still in the comment state, the comment was never closed[cite: 18]. */
<MULTI_COMMENT><<EOF>>  { has_error = 1; return 0; /* Unterminated multi-line comment error */ }


    /* --- 2. Single-line comments[cite: 14] --- */
    /* Matches "//" followed by zero or more of ANY character except a newline (.). 
       Because Lex rules are applied top-down and favor longest matches, this safely consumes the rest of the line. */
"//".*                  { single_comments++; }


    /* --- 3. Preprocessor directives[cite: 15] --- */
    /* ^ anchors the match to the start of a line. 
       [ \t]* allows optional whitespace before the '#'. 
       .* captures the rest of the directive[cite: 17]. */
^[ \t]*#.*              { preproc++; printf("%-18s %s\n", "PREPROCESSOR", yytext); }


    /* --- 4. String literals[cite: 15] --- */
    /* 
       REGEX BREAKDOWN: \"([^\\\n\"]|\\.)*\"
       - \"          : Matches the opening double quote.
       - ( ... )*    : Matches the contents zero or more times.
       - [^\\\n\"]   : Matches any character that is NOT a backslash, newline, or double quote.
       - |           : OR
       - \\.         : Matches an escaped character (e.g., \n, \t, or \"). This ensures a quote inside a string doesn't end it prematurely[cite: 19].
       - \"          : Matches the closing double quote.
       This structure ensures that '//' or '/*' inside a string is treated as text, not a comment[cite: 19].
    */
\"([^\\\n\"]|\\.)*\"    { strings++; printf("%-18s %s\n", "STRING LITERAL", yytext); }

    /* If we find a string that hits a newline or EOF without a closing quote, flag an error[cite: 18]. */
\"([^\\\n\"]|\\.)*      { has_error = 1; return 0; /* Unterminated string literal error */ }


    /* --- 5. Character literals[cite: 15] --- */
    /* Identical logic to string literals, but using single quotes (\') to capture items like 'a' or '\n'[cite: 17]. */
\'([^\\\n\']|\\.)*\'    { chars++; printf("%-18s %s\n", "CHAR LITERAL", yytext); }
\'([^\\\n\']|\\.)*      { has_error = 1; return 0; /* Unterminated character literal error */ }


    /* --- 6. Identifiers[cite: 16] --- */
    /* Standard C identifier rules: must start with a letter or underscore, followed by letters, digits, or underscores. */
[a-zA-Z_][a-zA-Z0-9_]*  { identifiers++; printf("%-18s %s\n", "IDENTIFIER", yytext); }


    /* --- 7. Line tracking and ignoring other characters[cite: 16] --- */
    /* If a newline isn't caught by a comment or string rule, count it here[cite: 17]. */
\n                      { lines++; }

    /* Consume and discard whitespace so it doesn't clutter output. */
[ \t\r]+                { /* Ignore whitespace */ }

    /* The catch-all rule (.). Any punctuation, brackets, or math operators fall here 
       and are silently ignored as required by the assignment[cite: 16]. */
.                       { /* Ignore all other ordinary source characters */ }

%%

int main() {
    // Start the lexical analysis
    yylex();

    // Print the final statistics gathered by the global counters[cite: 17]
    printf("\nSource Code Statistics\n\n");
    printf("Lines\t\t\t: %d\n", lines);
    printf("Comments\t\t: %d\n", single_comments + multi_comments);
    printf("String literals\t\t: %d\n", strings);
    printf("Character literals\t: %d\n", chars);
    printf("Preprocessor\t\t: %d\n", preproc);
    printf("Identifiers\t\t: %d\n\n", identifiers);

    /* Final Verdict based on whether our error rules were triggered[cite: 18] */
    if (has_error) {
        printf("REJECT\n");
    } else {
        printf("ACCEPT\n");
    }

    return 0;
}

// Called by Flex when it reaches the end of the input file. 
// Returning 1 tells Flex we are done and there are no more files to parse.
int yywrap() {
    return 1;
}