/* Compile: flex com.l && gcc lex.yy.c -o com -lfl */
/* Run with the sample input: ./com input.c */

/* 
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// Global State Trackers
int ln = 1;         // Line counter, incremented on every newline[cite: 28, 30]
int in_cmt = 0;     // Flag to track if we are currently inside a multi-line comment[cite: 29, 30]
int cmt_ln = 0;     // Records the line number where a multi-line comment started[cite: 30]

// Statistics Counters[cite: 27, 30]
int dec = 0, oct = 0, hex = 0, bin = 0, flt = 0, err = 0;

long long sum = 0;  // Sum of all valid integer values[cite: 28, 30]
long long mx = 0;   // Largest valid integer encountered[cite: 28, 30]
int has_int = 0;    // Flag to safely initialize the max value comparison[cite: 30]

// Helper function to process all integer bases (Binary, Octal, Decimal, Hex)
void handle_int(const char *s, const char *cat, int b, int off) {
    errno = 0; // Clear errno before conversion to accurately detect out-of-range limits[cite: 28, 30]
    
    // strtoll converts the string (skipping the prefix via 'off') into a long long using the specified base 'b'[cite: 28, 30]
    long long v = strtoll(s + off, NULL, b);
    
    // Range check: Identify values that overflow a long long[cite: 28, 30]
    if (errno == ERANGE) {
        printf("Lexical Error at line %d: integer literal '%s' out of range\n", ln, s);
        err++;
        return; // Do not count out-of-range literals in the statistics[cite: 28, 30]
    }
    
    printf("%d: %s -> %s (decimal value %lld)\n", ln, s, cat, v);
    
    // Update categorical statistics based on the base passed[cite: 30]
    if (b == 10) dec++;
    else if (b == 8) oct++;
    else if (b == 16) hex++;
    else if (b == 2) bin++;
    
    sum += v;
    if (!has_int || v > mx) {
        mx = v;
        has_int = 1;
    }
}

// Helper function to process floating point numbers
void handle_flt(const char *s) {
    errno = 0;
    double v = strtod(s, NULL); // Convert to double[cite: 28, 30]
    if (errno == ERANGE) {
        printf("Lexical Error at line %d: float literal '%s' out of range\n", ln, s);
        err++;
        return;
    }
    
    printf("%d: %s -> FLOAT (value %g)\n", ln, s, v);
    flt++;
}
%}

/* Exclusive start condition for handling multi-line comments safely[cite: 28, 30] */
%x COMMENT

/* 
 * Reusable Lex Definitions[cite: 28, 30]
 * These building blocks make the rules below much easier to read.
 */
DIGIT   [0-9]
NZ      [1-9]
OCT     [0-7]
HEX     [0-9a-fA-F]
BIN     [01]
EXP     [eE][+-]?{DIGIT}+

%%

    /* --- Comments --- */
"//".*                  { /* Discard single-line comments entirely so digits inside aren't parsed[cite: 26, 28, 30] */ }

"/*"                    { in_cmt = 1; cmt_ln = ln; BEGIN(COMMENT); /* Enter exclusive comment state[cite: 30] */ }
<COMMENT>"*/"           { in_cmt = 0; BEGIN(INITIAL); /* Exit comment state[cite: 30] */ }
<COMMENT>\n             { ln++; /* Maintain accurate line counts even inside comments[cite: 28, 30] */ }
<COMMENT>.              { /* Silently consume everything else inside the multi-line comment[cite: 30] */ }

    /* --- Strings and Characters --- */
    /* String literals: Match quotes, allowing escaped quotes (\") inside. Consume silently[cite: 28, 30]. */
\"([^\"\\\n]|\\.)*\"    { }
    /* Unterminated string literal detection (hits a newline without a closing quote)[cite: 28, 30] */
\"([^\"\\\n]|\\.)*      { 
                          printf("Lexical Error at line %d: unterminated string literal\n", ln); 
                          err++; 
                        }

    /* Character literals: Consume silently[cite: 28, 30]. */
\'([^\'\\\n]|\\.)*\'    { }

    /* --- Identifiers --- */
    /* Consume identifiers silently so numeric suffixes (like 'x1' or 'v0x1F') aren't treated as numbers[cite: 28, 30]. */
[A-Za-z_][A-Za-z0-9_]*  { }

    /* --- Valid Numeric Literals --- 
     * ORDERING IS CRITICAL: Lex matches the longest string, but if two rules match the same length, 
     * the first rule wins. Therefore, valid number rules MUST appear before the malformed rule[cite: 26, 28, 30]. 
     */
0[xX]{HEX}+             { handle_int(yytext, "HEX_INT", 16, 2); /* offset 2 to skip '0x'[cite: 26, 28, 30] */ }
0[bB]{BIN}+             { handle_int(yytext, "BINARY_INT", 2, 2); /* offset 2 to skip '0b'[cite: 26, 28, 30] */ }
0{OCT}+                 { handle_int(yytext, "OCTAL_INT", 8, 0); /* Base 8[cite: 26, 30] */ }
0|{NZ}{DIGIT}*          { handle_int(yytext, "DECIMAL_INT", 10, 0); /* 0 or a non-zero digit followed by digits[cite: 26, 30] */ }

    /* Floats: Matches standard decimal formats (e.g., 3.14) and scientific notation (e.g., 2.5e3)[cite: 27, 30] */
{DIGIT}+"."{DIGIT}+{EXP}? |
{DIGIT}+{EXP}           { handle_flt(yytext); }

    /* --- Malformed Numeric Literals --- */
    /* Catch-all for strings that START with a digit but contain invalid letters/symbols (e.g., 12abc, 08).
       Because this is placed after the valid rules, legitimate hex/float numbers will already be consumed[cite: 28, 30]. */
{DIGIT}+([eE][+-]?[A-Za-z0-9_.]*|[A-Za-z0-9_.]+) {
                          printf("Lexical Error at line %d: malformed numeric literal '%s'\n", ln, yytext);
                          err++;
                        }

    /* --- Catch-all & Formatting --- */
\n                      { ln++; /* Increment line counter on newlines outside of comments[cite: 28, 30] */ }
.                       { /* Silently ignore any stray characters not matched above[cite: 30] */ }

%%

/* yywrap is called by Lex at the end of the input file[cite: 28, 29] */
int yywrap(void) {
    // If the file ends while the comment flag is still active, the comment was unterminated[cite: 29, 30]
    if (in_cmt) {
        printf("Lexical Error at line %d: unterminated comment\n", cmt_ln);
        err++;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            perror(argv[1]);
            return 1;
        }
        yyin = fp; // Direct Lex to read from the specified file instead of stdin[cite: 30]
    }
    
    yylex(); // Start the scanning process[cite: 30]
    
    // Print the required statistics block at the end of input[cite: 27, 28, 30]
    printf("--- Numeric Literal Statistics ---\n");
    printf("Decimal integers     : %d\n", dec);
    printf("Octal integers       : %d\n", oct);
    printf("Hexadecimal integers : %d\n", hex);
    printf("Binary integers      : %d\n", bin);
    printf("Floating-point       : %d\n", flt);
    printf("Lexical errors       : %d\n", err);
    printf("Sum of integers      : %lld\n", sum);
    printf("Largest integer      : %lld\n", mx);
    
    // Final Output Verdict based on whether any errors were logged[cite: 28, 30]
    if (err == 0)
        printf("ACCEPT\n");
    else
        printf("REJECT\n");
    
    return 0;
}