// Compile: gcc ll1.c -o ll1

// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* --- Grammar Symbol Encodings --- */
// Terminals (0 to 6)
#define EPSILON 0
#define T_PLUS 1
#define T_STAR 2
#define T_LPAREN 3
#define T_RPAREN 4
#define T_ID 5
#define T_EOF 6

// Non-Terminals (7 to 11)[cite: 44]
#define NT_E 7
#define NT_EP 8
#define NT_T 9
#define NT_TP 10
#define NT_F 11

#define NUM_TERMINALS 7
#define NUM_NONTERMINALS 5
#define MAX_RHS 5

/* --- Data Structures --- */
// Represents a single grammar production rule[cite: 44]
typedef struct {
    int id;
    int lhs;              // Left-hand side Non-Terminal
    int rhs[MAX_RHS];     // Array of Right-hand side symbols
    int rhs_len;          // Number of symbols in RHS
    const char *prod_str; // String representation for output[cite: 44]
} Production;

Production prods[8];
int num_prods = 8;

// FIRST and FOLLOW sets: Boolean matrices where [NT_Index][Terminal] is true/false[cite: 45]
bool first[NUM_NONTERMINALS][NUM_TERMINALS];
bool follow[NUM_NONTERMINALS][NUM_TERMINALS];

// The LL(1) Parsing Table: Maps M[Non-Terminal][Terminal] -> Production ID[cite: 45]
int M[NUM_NONTERMINALS][NUM_TERMINALS];

/* --- Helper Functions --- */
// Maps terminal symbols to their string representation
const char* get_term_name(int t) {
    switch(t) {
        case T_PLUS: return "+";
        case T_STAR: return "*";
        case T_LPAREN: return "(";
        case T_RPAREN: return ")";
        case T_ID: return "id";
        case T_EOF: return "$";
        case EPSILON: return "epsilon";
        default: return "?";
    }
}

// Maps non-terminal symbols to their string representation
const char* get_nt_name(int nt) {
    switch(nt) {
        case NT_E: return "E";
        case NT_EP: return "E'";
        case NT_T: return "T";
        case NT_TP: return "T'";
        case NT_F: return "F";
        default: return "?";
    }
}

/* --- Initialization --- */
// Hard-codes the unambiguous, left-recursion-removed G2 LL(1) grammar[cite: 44, 45].
void init_grammar() {
    // E -> T E'
    prods[0] = (Production){0, NT_E, {NT_T, NT_EP}, 2, "E -> T E'"};
    // E' -> + T E'
    prods[1] = (Production){1, NT_EP, {T_PLUS, NT_T, NT_EP}, 3, "E' -> + T E'"};
    // E' -> epsilon
    prods[2] = (Production){2, NT_EP, {EPSILON}, 1, "E' -> epsilon"};
    // T -> F T'
    prods[3] = (Production){3, NT_T, {NT_F, NT_TP}, 2, "T -> F T'"};
    // T' -> * F T'
    prods[4] = (Production){4, NT_TP, {T_STAR, NT_F, NT_TP}, 3, "T' -> * F T'"};
    // T' -> epsilon
    prods[5] = (Production){5, NT_TP, {EPSILON}, 1, "T' -> epsilon"};
    // F -> ( E )
    prods[6] = (Production){6, NT_F, {T_LPAREN, NT_E, T_RPAREN}, 3, "F -> ( E )"};
    // F -> id
    prods[7] = (Production){7, NT_F, {T_ID}, 1, "F -> id"};
    
    // Initialize LL(1) table to -1 (empty)
    for (int i = 0; i < NUM_NONTERMINALS; i++)
        for (int j = 0; j < NUM_TERMINALS; j++)
            M[i][j] = -1;
}

/* --- FIRST & FOLLOW Set Computation[cite: 45] --- */
// Computes FIRST set for a given string of symbols (rhs sequence)
void get_first_of_string(int *str, int len, bool *res) {
    for (int i = 0; i < NUM_TERMINALS; i++) res[i] = false;
    
    if (len == 0 || str[0] == EPSILON) {
        res[EPSILON] = true;
        return;
    }
    
    for (int i = 0; i < len; i++) {
        int sym = str[i];
        if (sym < NUM_TERMINALS) { // If it's a Terminal
            res[sym] = true;
            return;
        } else { // If it's a Non-Terminal
            int nt_idx = sym - 7;
            bool derives_eps = false;
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (first[nt_idx][t] && t != EPSILON) res[t] = true;
                if (first[nt_idx][t] && t == EPSILON) derives_eps = true;
            }
            if (!derives_eps) return; // Stop if this NT cannot derive Epsilon
        }
    }
    res[EPSILON] = true; // All symbols can derive Epsilon
}

// Fixed-point iteration to compute FIRST sets[cite: 45]
void compute_first() {
    bool changed = true;
    while (changed) {
        changed = false;
        for (int p = 0; p < num_prods; p++) {
            int nt_idx = prods[p].lhs - 7;
            bool rhs_first[NUM_TERMINALS];
            get_first_of_string(prods[p].rhs, prods[p].rhs_len, rhs_first);
            
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (rhs_first[t] && !first[nt_idx][t]) {
                    first[nt_idx][t] = true;
                    changed = true;
                }
            }
        }
    }
}

// Fixed-point iteration to compute FOLLOW sets[cite: 45]
void compute_follow() {
    // 1. Place $ in FOLLOW(Start Symbol)
    follow[NT_E - 7][T_EOF] = true;
    
    bool changed = true;
    while (changed) {
        changed = false;
        // Process every production[cite: 45]
        for (int p = 0; p < num_prods; p++) {
            int lhs_idx = prods[p].lhs - 7;
            int len = prods[p].rhs_len;
            
            for (int i = 0; i < len; i++) {
                int sym = prods[p].rhs[i];
                if (sym >= 7) { // If symbol is a Non-Terminal
                    int sym_idx = sym - 7;
                    bool beta_first[NUM_TERMINALS];
                    get_first_of_string(&prods[p].rhs[i + 1], len - (i + 1), beta_first);
                    
                    // Add FIRST(Beta) \ {Epsilon} to FOLLOW(sym)
                    for (int t = 1; t < NUM_TERMINALS; t++) {
                        if (beta_first[t] && !follow[sym_idx][t]) {
                            follow[sym_idx][t] = true;
                            changed = true;
                        }
                    }
                    // If Beta can derive Epsilon, add FOLLOW(LHS) to FOLLOW(sym)[cite: 45]
                    if (beta_first[EPSILON]) {
                        for (int t = 0; t < NUM_TERMINALS; t++) {
                            if (follow[lhs_idx][t] && !follow[sym_idx][t]) {
                                follow[sym_idx][t] = true;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

/* --- LL(1) Table Construction[cite: 45] --- */
void build_table() {
    for (int p = 0; p < num_prods; p++) {
        int nt_idx = prods[p].lhs - 7;
        bool rhs_first[NUM_TERMINALS];
        get_first_of_string(prods[p].rhs, prods[p].rhs_len, rhs_first);
        
        // For each terminal 'a' in FIRST(alpha), add A -> alpha to M[A, a][cite: 45]
        for (int t = 1; t < NUM_TERMINALS; t++) {
            if (rhs_first[t]) {
                // LL(1) Conflict Detection requirement[cite: 45]
                if (M[nt_idx][t] != -1 && M[nt_idx][t] != p) {
                    printf("LL(1) conflict at M[%s, %s]\n", get_nt_name(prods[p].lhs), get_term_name(t));
                }
                M[nt_idx][t] = p;
            }
        }
        
        // If Epsilon is in FIRST(alpha), add A -> alpha to M[A, b] for each 'b' in FOLLOW(A)[cite: 45]
        if (rhs_first[EPSILON]) {
            for (int t = 1; t < NUM_TERMINALS; t++) {
                if (follow[nt_idx][t]) {
                    if (M[nt_idx][t] != -1 && M[nt_idx][t] != p) {
                        printf("LL(1) conflict at M[%s, %s]\n", get_nt_name(prods[p].lhs), get_term_name(t));
                    }
                    M[nt_idx][t] = p;
                }
            }
        }
    }
}

/* --- Lexical Analyzer (Scanner)[cite: 45] --- */
int next_token(const char **input) {
    while (isspace(**input)) (*input)++;
    
    if (**input == '\0') return T_EOF;
    if (**input == '+') { (*input)++; return T_PLUS; }
    if (**input == '*') { (*input)++; return T_STAR; }
    if (**input == '(') { (*input)++; return T_LPAREN; }
    if (**input == ')') { (*input)++; return T_RPAREN; }
    if (strncmp(*input, "id", 2) == 0) { (*input) += 2; return T_ID; }
    
    return -1; // Lexical Error
}

/* --- LL(1) Predictive Parser --- */
void parse(const char *input) {
    printf("ll1: parsing with G2 using LL(1) table...\n");
    
    int stack[1000];
    int top = 0;
    
    // Stack initially contains $ and the Start Symbol (E)[cite: 45]
    stack[top++] = T_EOF;
    stack[top++] = NT_E;
    
    const char *ptr = input;
    int lookahead = next_token(&ptr);
    
    while (top > 0) {
        int X = stack[--top];
        
        if (X < NUM_TERMINALS) { // If X is a Terminal
            if (X == lookahead) {
                lookahead = next_token(&ptr); // Match successful, advance token stream
            } else {
                printf("REJECT\n"); // Mismatch[cite: 45]
                return;
            }
        } else { // If X is a Non-Terminal
            int nt_idx = X - 7;
            int p_idx = M[nt_idx][lookahead];
            
            if (p_idx != -1) {
                printf("%s\n", prods[p_idx].prod_str); // Print the sequence of applied productions[cite: 45]
                
                // Push RHS onto stack in reverse order
                if (prods[p_idx].rhs[0] != EPSILON) {
                    for (int i = prods[p_idx].rhs_len - 1; i >= 0; i--) {
                        stack[top++] = prods[p_idx].rhs[i];
                    }
                }
            } else {
                // Table cell is empty, meaning syntax error
                printf("REJECT\n");
                return;
            }
        }
    }
    
    if (lookahead == T_EOF) {
        printf("ACCEPT\n"); // Final verdict requirement[cite: 45]
    } else {
        printf("REJECT\n");
    }
}

int main() {
    char input[1024];
    
    // Build the underlying parsing constructs
    init_grammar();
    compute_first();
    compute_follow();
    build_table();
    
    // Read user input
    if (fgets(input, sizeof(input), stdin)) {
        // Trim trailing newline
        input[strcspn(input, "\n")] = 0;
        parse(input);
    }
    
    return 0;
}