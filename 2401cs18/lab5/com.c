// Compile: gcc com.c -o com
// Run with the sample input: ./com < input.txt

// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>   // POSIX regex library required for manual pattern matching
#include <stdbool.h>

// Helper function to check if an identifier is a keyword.
// This resolves the keyword vs. identifier conflict[cite: 10, 11]. We first recognize a generic
// identifier (e.g., "if" or "sum"), then cross-reference it against this list[cite: 11].
bool check_kw(const char *text) {
    const char *kw_list[] = {"if", "else", "while", "int", "string", "return"};
    for (int i = 0; i < 6; i++) {
        if (strcmp(text, kw_list[i]) == 0) return true;
    }
    return false;
}

// Helper function to map generic token categories to specific output classes[cite: 10].
// For example, mapping a RELOP token to "LESS_EQ" or an ARITHMETIC token to "PLUS"[cite: 9, 13].
void print_tok(const char *tok_type, const char *lex) {
    if (strcmp(tok_type, "IDENTIFIER") == 0) {
        if (check_kw(lex)) {
            printf("KEYWORD %s\n", lex); // It matched a keyword[cite: 10]
        } else {
            printf("IDENTIFIER %s\n", lex);
        }
    } else if (strcmp(tok_type, "REAL") == 0) {
        printf("REAL %s\n", lex);
    } else if (strcmp(tok_type, "INTEGER") == 0) {
        printf("INTEGER %s\n", lex);
    } else if (strcmp(tok_type, "RELOP") == 0) {
        if (strcmp(lex, "<") == 0) printf("LESS %s\n", lex);
        else if (strcmp(lex, "<=") == 0) printf("LESS_EQ %s\n", lex);
        else if (strcmp(lex, ">") == 0) printf("GREATER %s\n", lex);
        else if (strcmp(lex, ">=") == 0) printf("GREATER_EQ %s\n", lex);
        else if (strcmp(lex, "==") == 0) printf("EQUAL %s\n", lex);
        else if (strcmp(lex, "!=") == 0) printf("NOT_EQUAL %s\n", lex);
    } else if (strcmp(tok_type, "ASSIGN") == 0) {
        printf("ASSIGN %s\n", lex);
    } else if (strcmp(tok_type, "ARITHMETIC") == 0) {
        if (strcmp(lex, "+") == 0) printf("PLUS %s\n", lex);
        else if (strcmp(lex, "-") == 0) printf("MINUS %s\n", lex);
        else if (strcmp(lex, "*") == 0) printf("MULT %s\n", lex);
        else if (strcmp(lex, "/") == 0) printf("DIV %s\n", lex);
    } else if (strcmp(tok_type, "DELIMITER") == 0) {
        if (strcmp(lex, "(") == 0) printf("LPAREN %s\n", lex);
        else if (strcmp(lex, ")") == 0) printf("RPAREN %s\n", lex);
        else if (strcmp(lex, "{") == 0) printf("LBRACE %s\n", lex);
        else if (strcmp(lex, "}") == 0) printf("RBRACE %s\n", lex);
        else if (strcmp(lex, ";") == 0) printf("SEMICOLON %s\n", lex);
        else if (strcmp(lex, ",") == 0) printf("COMMA %s\n", lex);
    }
}

// Structure to bind a logical token name to its raw string regex pattern and compiled regex object.
typedef struct {
    const char *name;
    const char *regex_str;
    regex_t compiled_reg; // Holds the machine-readable compiled regex created by regcomp()
} RuleDef;

int main() {
    // Regular expression token definitions[cite: 10].
    // CRITICAL: Every pattern is anchored with '^'. This forces the regex engine to start 
    // matching exactly at the current pointer position in the input string, rather than 
    // skipping ahead to find a match later in the string[cite: 13].
    RuleDef token_rules[] = {
        {"WHITESPACE", "^[ \t\n\r]+", {0}},
        {"REAL", "^[0-9]+\\.[0-9]+", {0}},          // Decimal numbers (e.g., 25.50)[cite: 8, 11]
        {"INTEGER", "^[0-9]+", {0}},                // Whole numbers (e.g., 25). Must follow REAL to prevent partial matching[cite: 8, 11].
        {"IDENTIFIER", "^[a-zA-Z][a-zA-Z0-9_]*", {0}}, // Starts with letter, followed by letters/digits/underscores[cite: 8]
        {"RELOP", "^(<=|>=|==|!=|<|>)", {0}},       // Multi-character operators must be listed before single characters[cite: 9]
        {"ASSIGN", "^=", {0}},
        {"ARITHMETIC", "^[+*/-]", {0}},             // Basic math operators[cite: 9]
        {"DELIMITER", "^[(){};,]", {0}}             // Punctuation and brackets[cite: 9]
    };
    int rule_count = sizeof(token_rules) / sizeof(token_rules[0]);

    // Compile regex patterns into internal formats using POSIX regcomp().
    // REG_EXTENDED enables modern regex syntax (like '+', '()', and '|').
    for (int i = 0; i < rule_count; i++) {
        if (regcomp(&token_rules[i].compiled_reg, token_rules[i].regex_str, REG_EXTENDED) != 0) {
            fprintf(stderr, "Error in regex: %s\n", token_rules[i].regex_str);
            return 1;
        }
    }

    // Read the source program from standard input into a buffer[cite: 10].
    char src_buffer[10000];
    size_t bytes_read = fread(src_buffer, 1, sizeof(src_buffer) - 1, stdin);
    src_buffer[bytes_read] = '\0'; // Null-terminate the input string

    char *ptr = src_buffer; // Pointer tracks our current position in the source code
    bool has_error = false;

    // Scan input sequentially from left to right[cite: 11]
    while (*ptr != '\0') {
        int max_len = 0;
        int best_idx = -1;
        regmatch_t m; // Struct populated by regexec() with the start and end offsets of a match

        // Test every regex rule against the string starting at 'ptr'.
        // This loop enforces the longest-match rule[cite: 10, 11].
        for (int i = 0; i < rule_count; i++) {
            // Execute regex match. '1' means we only care about the primary match (m[0]).
            if (regexec(&token_rules[i].compiled_reg, ptr, 1, &m, 0) == 0) {
                // Because of the '^' anchor, a valid match MUST start at offset 0 (m.rm_so == 0).
                if (m.rm_so == 0) { 
                    int cur_len = m.rm_eo; // Length of the matched lexeme
                    // Longest-match principle: update our selection if this rule matched more characters[cite: 10, 11].
                    // E.g., for ">=", RELOP matches 2 characters, breaking a 1-character match for ">"[cite: 10].
                    if (cur_len > max_len) {
                        max_len = cur_len;
                        best_idx = i;
                    }
                }
            }
        }

        // Lexical error detection[cite: 10].
        // If no regex rules matched the character at 'ptr', we have an invalid symbol (like '@')[cite: 10].
        if (best_idx == -1) {
            printf("LEXICAL ERROR: unexpected character '%c'\n", *ptr); // Identify offending character[cite: 10]
            has_error = true;
            break; // Stop scanning when an error is encountered[cite: 11]
        }

        // If a token was recognized, check if it's whitespace.
        // Whitespace must be ignored and not produce an output token[cite: 10].
        if (strcmp(token_rules[best_idx].name, "WHITESPACE") != 0) {
            char temp_lex[256];
            // Extract the exact matched lexeme into a temporary buffer
            strncpy(temp_lex, ptr, max_len);
            temp_lex[max_len] = '\0';
            // Print the token class and lexeme[cite: 10]
            print_tok(token_rules[best_idx].name, temp_lex);
        }

        // Advance the pointer by the length of the matched lexeme to process the next chunk of code
        ptr += max_len;
    }

    // Final verdict[cite: 10]
    if (has_error) {
        printf("REJECT\n");
    } else {
        printf("ACCEPT\n");
    }

    // Free compiled regexes to prevent memory leaks
    for (int i = 0; i < rule_count; i++) {
        regfree(&token_rules[i].compiled_reg);
    }

    return 0;
}