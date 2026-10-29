/*
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

%{
/* 
 * SECTION 1: C Declarations 
 * These headers and global variables are copied directly into the generated Lab10.tab.c file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

int yylex(void);
void yyerror(const char *s);

int line_num = 1;      // Tracks line numbers for syntax error reporting[cite: 35]
int temp_count = 0;    // Counter for intermediate TAC temporary variables (e.g., t1, t2)[cite: 33]
int label_count = 0;   // Counter for control flow labels (e.g., L1, L2)[cite: 33]
int has_error = 0;     // Flag to halt TAC output if a syntax error occurs[cite: 35]

/* 
 * TAC Buffering System
 * The assignment strictly requires that invalid programs must NOT produce partial TAC[cite: 35].
 * Instead of printing immediately, emit() stores the TAC strings in memory. 
 * They are only printed at the very end if parsing succeeds perfectly.
 */
#define MAX_TAC_LINES 10000
char *tac_buffer[MAX_TAC_LINES];
int tac_index = 0;

// emit() acts like printf but writes to our TAC buffer.
void emit(const char *fmt, ...) {
    if (has_error) return; // Stop processing if a syntax error was found
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (tac_index < MAX_TAC_LINES) {
        tac_buffer[tac_index++] = strdup(buf);
    }
}

// Generates a new temporary variable string (t1, t2, t3...)[cite: 33]
char* new_temp() {
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", ++temp_count);
    return strdup(buf);
}

// Generates a new unique label integer (1, 2, 3...)[cite: 33]
int new_label() {
    return ++label_count;
}

// Struct to track jump targets for conditional statements (if/else)[cite: 34]
typedef struct {
    int l_true;
    int l_false;
    int l_end;
} CondLabels;
%}

/* 
 * SECTION 2: Yacc Declarations
 * Defines the types of values that can be passed between grammar rules.
 */
%union {
    char *str;   // Used for passing variable names, numbers, and TAC strings
    void *ptr;   // Used for passing the CondLabels struct for control flow
}

// Token definitions with associated types from the %union above
%token <str> ID NUM
%token IF ELSE
%token LE GE EQ NE

/* 
 * Operator Precedence and Associativity[cite: 31, 33]
 * Yacc resolves shift/reduce conflicts based on the order of these declarations.
 * Lower lines have HIGHER precedence.
 */
%left '+' '-'        // Addition/Subtraction are left-associative[cite: 33]
%left '*' '/'        // Multiplication/Division have higher precedence than +/-[cite: 33]
%right UMINUS        // Unary minus has the highest arithmetic precedence[cite: 33]

/* 
 * Resolve Dangling-Else Ambiguity[cite: 31, 33]
 * When the parser sees an 'if' without an 'else', and an 'else' is approaching in the lookahead,
 * it must decide whether to reduce the 'if' or shift the 'else' to pair it with the nearest 'if'[cite: 33].
 * Giving ELSE a higher precedence than LOWER_THAN_ELSE forces Yacc to SHIFT the 'else', 
 * correctly binding it to the innermost unmatched 'if'[cite: 33].
 */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

// Declare the return types for non-terminals
%type <str> expr rel_expr
%type <ptr> if_prefix

%%

/* 
 * SECTION 3: Grammar Rules & Semantic Actions
 * When a pattern is matched, the C code inside { } is executed to generate TAC[cite: 33].
 */

program:
    stmt_list {
        // Output the buffered TAC only if the entire file was parsed without syntax errors[cite: 35]
        if (!has_error) {
            for (int i = 0; i < tac_index; i++) {
                printf("%s\n", tac_buffer[i]);
                free(tac_buffer[i]);
            }
        }
    }
    ;

stmt_list:
    stmt_list stmt
    | /* empty */
    ;

stmt:
    assign_stmt
    | if_stmt
    | block
    ;

/* Assignment Statement[cite: 31] */
assign_stmt:
    ID '=' expr ';' {
        emit("%s = %s", $1, $3); // e.g., a = t2[cite: 34]
        free($1);
        free($3);
    }
    ;

/* Nested Blocks[cite: 33] */
block:
    '{' stmt_list '}'
    ;

/* 
 * Conditional Control Flow (Mid-Rule Action)
 * When Yacc sees "if ( condition )", we must generate the conditional jumps 
 * BEFORE we parse the body of the if-statement[cite: 33, 34]. 
 */
if_prefix:
    IF '(' rel_expr ')' {
        CondLabels *lbl = (CondLabels *)malloc(sizeof(CondLabels));
        lbl->l_true = new_label();
        lbl->l_false = new_label();
        lbl->l_end = 0;
        
        // Generate TAC jumps for the condition[cite: 34]
        emit("if %s goto L%d", $3, lbl->l_true);
        emit("goto L%d", lbl->l_false);
        emit("L%d:", lbl->l_true); // Start of the "True" block[cite: 34]
        
        free($3);
        $$ = (void *)lbl; // Pass the label data up the parsing tree
    }
    ;

/* Conditional Statements[cite: 32] */
if_stmt:
    // Basic if-statement without else (uses %prec to force lower priority than an approaching ELSE)[cite: 33]
    if_prefix stmt %prec LOWER_THAN_ELSE {
        CondLabels *lbl = (CondLabels *)$1;
        emit("L%d:", lbl->l_false); // Print the false label to skip the if-block
        free(lbl);
    }
    // If-Else statement[cite: 32]
    | if_prefix stmt ELSE {
        CondLabels *lbl = (CondLabels *)$1;
        lbl->l_end = new_label();
        
        // At the end of the true block, we must jump over the else block[cite: 34, 35]
        emit("goto L%d", lbl->l_end);
        
        // Start of the false (else) block[cite: 35]
        emit("L%d:", lbl->l_false);
    } stmt {
        CondLabels *lbl = (CondLabels *)$1;
        emit("L%d:", lbl->l_end); // End of the entire if-else construct
        free(lbl);
    }
    ;

/* Relational Expressions (e.g., a < b)[cite: 32] */
rel_expr:
    expr '<' expr {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s < %s", $1, $3);
        $$ = strdup(buf);
        free($1); free($3);
    }
    | expr '>' expr {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s > %s", $1, $3);
        $$ = strdup(buf);
        free($1); free($3);
    }
    | expr LE expr {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s <= %s", $1, $3);
        $$ = strdup(buf);
        free($1); free($3);
    }
    | expr GE expr {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s >= %s", $1, $3);
        $$ = strdup(buf);
        free($1); free($3);
    }
    | expr EQ expr {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s == %s", $1, $3);
        $$ = strdup(buf);
        free($1); free($3);
    }
    | expr NE expr {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s != %s", $1, $3);
        $$ = strdup(buf);
        free($1); free($3);
    }
    ;

/* Arithmetic Expressions[cite: 32] */
expr:
    expr '+' expr {
        char *t = new_temp(); // Request a temporary variable[cite: 33]
        emit("%s = %s + %s", t, $1, $3); // Generate intermediate TAC (e.g., t1 = a + b)[cite: 34]
        $$ = t; // Pass the temporary variable up the tree
        free($1); free($3);
    }
    | expr '-' expr {
        char *t = new_temp();
        emit("%s = %s - %s", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | expr '*' expr {
        char *t = new_temp();
        emit("%s = %s * %s", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | expr '/' expr {
        char *t = new_temp();
        emit("%s = %s / %s", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | '-' expr %prec UMINUS {
        char *t = new_temp();
        emit("%s = -%s", t, $2);
        $$ = t;
        free($2);
    }
    | '(' expr ')' {
        $$ = $2; // Parentheses are bypassed, passing the inner expression result upwards[cite: 32, 34]
    }
    | ID {
        $$ = $1; // Base case: Variables
    }
    | NUM {
        $$ = $1; // Base case: Numbers
    }
    ;

%%

/* 
 * SECTION 4: Custom Lexer (yylex) and Main 
 * Hand-written lexer in C since a separate Lex/Flex file was not strictly required[cite: 35, 36].
 */
int yylex(void) {
    int c;
    while ((c = getchar()) != EOF) {
        // Skip whitespace
        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }
        
        // Track line numbers for syntax error reporting[cite: 35]
        if (c == '\n') {
            line_num++;
            continue;
        }
        
        /* Handle single-line and multi-line comments manually */
        if (c == '/') {
            int next = getchar();
            if (next == '/') {
                while ((c = getchar()) != EOF && c != '\n');
                if (c == '\n') line_num++;
                continue;
            } else if (next == '*') {
                int prev = 0;
                while ((c = getchar()) != EOF) {
                    if (c == '\n') line_num++;
                    if (prev == '*' && c == '/') break;
                    prev = c;
                }
                continue;
            } else {
                ungetc(next, stdin);
            }
        }
        
        /* Identifiers and Keywords */
        if (isalpha(c) || c == '_') {
            char buf[128];
            int i = 0;
            buf[i++] = c;
            while ((c = getchar()) != EOF && (isalnum(c) || c == '_')) {
                if (i < 127) buf[i++] = c;
            }
            buf[i] = '\0';
            if (c != EOF) ungetc(c, stdin); // Push back the character that broke the loop
            
            // Check for reserved keywords[cite: 32, 37]
            if (strcmp(buf, "if") == 0) return IF;
            if (strcmp(buf, "else") == 0) return ELSE;
            
            // It's a standard identifier
            yylval.str = strdup(buf);
            return ID;
        }
        
        /* Numeric constants */
        if (isdigit(c)) {
            char buf[128];
            int i = 0;
            buf[i++] = c;
            while ((c = getchar()) != EOF && isdigit(c)) {
                if (i < 127) buf[i++] = c;
            }
            buf[i] = '\0';
            if (c != EOF) ungetc(c, stdin);
            
            yylval.str = strdup(buf);
            return NUM;
        }
        
        /* Two-character Relational Operators (<=, >=, ==, !=)[cite: 32] */
        if (c == '<') {
            int next = getchar();
            if (next == '=') return LE;
            if (next != EOF) ungetc(next, stdin);
            return '<';
        }
        if (c == '>') {
            int next = getchar();
            if (next == '=') return GE;
            if (next != EOF) ungetc(next, stdin);
            return '>';
        }
        if (c == '=') {
            int next = getchar();
            if (next == '=') return EQ;
            if (next != EOF) ungetc(next, stdin);
            return '=';
        }
        if (c == '!') {
            int next = getchar();
            if (next == '=') return NE;
            if (next != EOF) ungetc(next, stdin);
            return '!';
        }
        
        // Return single-character symbols (like '+', '-', ';', '{') directly to Yacc
        return c;
    }
    return 0; // EOF signals the end of token generation
}

// yyerror is called automatically by Yacc when grammar rules fail (e.g., missing semicolons, unbalanced braces)[cite: 35]
void yyerror(const char *s) {
    (void)s;
    has_error = 1; // Trigger flag to halt TAC printing[cite: 35]
    printf("Syntax error at line %d\n", line_num); // Report error with corresponding line number[cite: 35]
}

int main(void) {
    // Start parsing. Returns 1 to the OS if parsing completely fails or if our error flag was triggered.
    if (yyparse() != 0 || has_error) {
        return 1;
    }
    return 0;
}