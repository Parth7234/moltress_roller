/* 
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int ln = 1;
int in_cmt = 0;
int cmt_ln = 0;

int dec = 0, oct = 0, hex = 0, bin = 0, flt = 0, err = 0;

long long sum = 0;
long long mx = 0;
int has_int = 0;

void handle_int(const char *s, const char *cat, int b, int off) {
    errno = 0;
    long long v = strtoll(s + off, NULL, b);
    if (errno == ERANGE) {
        printf("Lexical Error at line %d: integer literal '%s' out of range\n", ln, s);
        err++;
        return;
    }
    
    printf("%d: %s -> %s (decimal value %lld)\n", ln, s, cat, v);
    
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

void handle_flt(const char *s) {
    errno = 0;
    double v = strtod(s, NULL);
    if (errno == ERANGE) {
        printf("Lexical Error at line %d: float literal '%s' out of range\n", ln, s);
        err++;
        return;
    }
    
    printf("%d: %s -> FLOAT (value %g)\n", ln, s, v);
    flt++;
}
%}

%x COMMENT

DIGIT   [0-9]
NZ      [1-9]
OCT     [0-7]
HEX     [0-9a-fA-F]
BIN     [01]
EXP     [eE][+-]?{DIGIT}+

%%

"//".*                  { }

"/*"                    { in_cmt = 1; cmt_ln = ln; BEGIN(COMMENT); }
<COMMENT>"*/"           { in_cmt = 0; BEGIN(INITIAL); }
<COMMENT>\n             { ln++; }
<COMMENT>.              { }

\"([^\"\\\n]|\\.)*\"    { }
\"([^\"\\\n]|\\.)*      { 
                          printf("Lexical Error at line %d: unterminated string literal\n", ln); 
                          err++; 
                        }

\'([^\'\\\n]|\\.)*\'    { }

[A-Za-z_][A-Za-z0-9_]*  { }

0[xX]{HEX}+             { handle_int(yytext, "HEX_INT", 16, 2); }
0[bB]{BIN}+             { handle_int(yytext, "BINARY_INT", 2, 2); }
0{OCT}+                 { handle_int(yytext, "OCTAL_INT", 8, 0); }
0|{NZ}{DIGIT}*          { handle_int(yytext, "DECIMAL_INT", 10, 0); }

{DIGIT}+"."{DIGIT}+{EXP}? |
{DIGIT}+{EXP}           { handle_flt(yytext); }

{DIGIT}+([eE][+-]?[A-Za-z0-9_.]*|[A-Za-z0-9_.]+) {
                          printf("Lexical Error at line %d: malformed numeric literal '%s'\n", ln, yytext);
                          err++;
                        }

\n                      { ln++; }
.                       { }

%%

int yywrap(void) {
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
        yyin = fp;
    }
    
    yylex();
    
    printf("--- Numeric Literal Statistics ---\n");
    printf("Decimal integers     : %d\n", dec);
    printf("Octal integers       : %d\n", oct);
    printf("Hexadecimal integers : %d\n", hex);
    printf("Binary integers      : %d\n", bin);
    printf("Floating-point       : %d\n", flt);
    printf("Lexical errors       : %d\n", err);
    printf("Sum of integers      : %lld\n", sum);
    printf("Largest integer      : %lld\n", mx);
    
    if (err == 0)
        printf("ACCEPT\n");
    else
        printf("REJECT\n");
    
    return 0;
}