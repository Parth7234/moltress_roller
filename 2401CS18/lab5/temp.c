// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

// Helper function to check if an identifier is a keyword
bool check_kw(const char *text) {
    const char *kw_list[] = {"if", "else", "while", "int", "string", "return"};
    for (int i = 0; i < 6; i++) {
        if (strcmp(text, kw_list[i]) == 0) return true;
    }
    return false;
}

// Helper function to print the specific token names
void print_tok(const char *tok_type, const char *lex) {
    if (strcmp(tok_type, "IDENTIFIER") == 0) {
        if (check_kw(lex)) {
            printf("KEYWORD %s\n", lex);
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

// Structure to hold token regex definitions
typedef struct {
    const char *name;
    const char *regex_str;
    regex_t compiled_reg;
} RuleDef;

int main() {
    // Regular expression token definitions
    // Patterns are anchored to the start of the string (^) to match at the current ptr
    RuleDef token_rules[] = {
        {"WHITESPACE", "^[ \t\n\r]+", {0}},
        {"REAL", "^[0-9]+\\.[0-9]+", {0}},
        {"INTEGER", "^[0-9]+", {0}},
        {"IDENTIFIER", "^[a-zA-Z][a-zA-Z0-9_]*", {0}},
        {"RELOP", "^(<=|>=|==|!=|<|>)", {0}},
        {"ASSIGN", "^=", {0}},
        {"ARITHMETIC", "^[+*/-]", {0}},
        {"DELIMITER", "^[(){};,]", {0}}
    };
    int rule_count = sizeof(token_rules) / sizeof(token_rules[0]);

    // Compile regex patterns
    for (int i = 0; i < rule_count; i++) {
        if (regcomp(&token_rules[i].compiled_reg, token_rules[i].regex_str, REG_EXTENDED) != 0) {
            fprintf(stderr, "Error in regex: %s\n", token_rules[i].regex_str);
            return 1;
        }
    }

    // Read the source program from standard input
    char src_buffer[10000];
    size_t bytes_read = fread(src_buffer, 1, sizeof(src_buffer) - 1, stdin);
    src_buffer[bytes_read] = '\0';

    char *ptr = src_buffer;
    bool has_error = false;

    // Scan input from left to right
    while (*ptr != '\0') {
        int max_len = 0;
        int best_idx = -1;
        regmatch_t m;

        // Apply longest-match rule
        for (int i = 0; i < rule_count; i++) {
            if (regexec(&token_rules[i].compiled_reg, ptr, 1, &m, 0) == 0) {
                if (m.rm_so == 0) { 
                    int cur_len = m.rm_eo;
                    if (cur_len > max_len) {
                        max_len = cur_len;
                        best_idx = i;
                    }
                }
            }
        }

        // Lexical error detection
        if (best_idx == -1) {
            printf("LEXICAL ERROR: unexpected character '%c'\n", *ptr);
            has_error = true;
            break; // Stop scanning when an error is encountered
        }

        // Output recognized tokens, ignoring whitespace
        if (strcmp(token_rules[best_idx].name, "WHITESPACE") != 0) {
            char temp_lex[256];
            strncpy(temp_lex, ptr, max_len);
            temp_lex[max_len] = '\0';
            print_tok(token_rules[best_idx].name, temp_lex);
        }

        // Advance ptr
        ptr += max_len;
    }

    // Final verdict
    if (has_error) {
        printf("REJECT\n");
    } else {
        printf("ACCEPT\n");
    }

    // Free compiled regexes
    for (int i = 0; i < rule_count; i++) {
        regfree(&token_rules[i].compiled_reg);
    }

    return 0;
}