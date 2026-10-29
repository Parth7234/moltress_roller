/* Compile: flex com.l && gcc lex.yy.c -o com -lfl */
/* Run with the sample inputs: ./com < input.c  or  ./com < input2.c */

%{
/* Name: Parth Singla */
/* Roll Number: 2401CS18 */

/* 
 * SECTION 1: C Declarations 
 * Everything inside %{ and %} is copied verbatim directly into the generated lex.yy.c file.
 * This is where we include standard C libraries needed for I/O and standard functions.
 */
#include <stdio.h>
#include <stdlib.h>
%}

/* 
 * DEFINITIONS SECTION
 * Declare an exclusive start condition (state) named COMMENT using %x[cite: 5].
 * "Exclusive" means that when the scanner enters this state, it will ONLY match rules 
 * that are explicitly prefixed with <COMMENT>[cite: 5]. It ignores all normal token rules, safely 
 * skipping quotes, braces, or other symbols inside the comment[cite: 5].
 */
%x COMMENT

%%
    /* 
     * SECTION 2: Rules Section (Pattern { Action })
     * Flex matches the longest possible string. If two rules match the same length, 
     * Flex prefers the rule that appears FIRST in this list[cite: 5].
     */

    /* --- Comment Skipping Rules --- */
    
    /* Single-line comments: Match "//" followed by zero or more characters that are NOT a newline ([^\n]*).
       The action is empty {}, meaning the matched text is silently consumed and discarded[cite: 4, 5]. */
"//"([^\n])*             { }

    /* Multi-line comments: When the opening comment delimiter is encountered, switch to the COMMENT state[cite: 5]. */
"/*"                     { BEGIN(COMMENT); }

    /* Inside the COMMENT state, the closing comment delimiter switches back to the INITIAL state[cite: 5]. */
<COMMENT>"*/"            { BEGIN(INITIAL); }

    /* While inside the <COMMENT> state, consume newlines. This ensures multi-line comments spanning 
       several lines are fully processed without leaking into the output[cite: 5]. */
<COMMENT>\n              { }

    /* While inside the <COMMENT> state, consume any other single character (.)[cite: 5]. */
<COMMENT>.               { }

    /* Error Handling: If the file ends (<<EOF>>) while still stuck inside the COMMENT state, 
       it means a "/*" was never closed. Print an error and terminate[cite: 5]. */
<COMMENT><<EOF>>         { printf("Error: unterminated comment\n"); return 0; }


    /* --- Whitespace --- */
    
    /* Match one or more (+) spaces, tabs (\t), newlines (\n), or carriage returns (\r).
       The empty action {} discards them so they don't break our token stream. */
[ \t\n\r]+               { }


    /* --- Token Classification Rules --- */
    
    /* KEYWORDS: 
       CRITICAL ORDERING: Keywords must be listed BEFORE the generic identifier rule[cite: 5].
       Since "int" matches both the keyword rule and the identifier rule with the same length (3 chars),
       Flex will pick this rule because it appears first[cite: 5]. 
       yytext is a built-in Flex string pointer containing the exact text that was matched. */
"int"|"float"|"char"|"double"|"if"|"else"|"while"|"for"|"return"|"void" { 
    printf("%s -> KEYWORD\n", yytext); 
}

    /* IDENTIFIERS: 
       Must start with a letter ([a-zA-Z]), followed by zero or more (*) letters or digits ([a-zA-Z0-9]*).
       Since keywords are already filtered out above, anything reaching this rule is a true identifier[cite: 5]. */
[a-zA-Z][a-zA-Z0-9]*     { printf("%s -> IDENTIFIER\n", yytext); }

    /* NUMBERS: 
       Matches integers (one or more digits: [0-9]+).
       Optionally matches a decimal part: (\.[0-9]+)? where the '?' means zero or one occurrence. */
[0-9]+(\.[0-9]+)?        { printf("%s -> NUMBER\n", yytext); }

    /* OPERATORS: 
       CRITICAL ORDERING: Multi-character operators (==, <=) must be listed BEFORE single-character 
       prefixes (=, <)[cite: 5]. Otherwise, given "==", Flex might match "=" twice instead of "==" once[cite: 5]. */
"=="|"<="|">="|"!="      { printf("%s -> OPERATOR\n", yytext); }
"+"|"-"|"*"|"/"|"="|"<"|">" { printf("%s -> OPERATOR\n", yytext); }

    /* PUNCTUATION: 
       Matches specific structural characters. */
";"|","|"("|")"|"{"|"}"  { printf("%s -> PUNCTUATION\n", yytext); }

    /* CATCH-ALL: 
       A single dot (.) matches any character not matched by the rules above. 
       The empty action ignores stray/invalid characters safely. */
.                        { }

%%

/* 
 * SECTION 3: User Code 
 * This contains the main driver program.
 */
int main(int argc, char **argv) {
    // yylex() starts the scanning process, reading from standard input by default.
    // It will keep matching rules until it hits the end of the file (EOF).
    yylex();
    return 0;
}