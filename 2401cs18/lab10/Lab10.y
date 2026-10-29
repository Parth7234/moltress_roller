/*
 * Name: Parth Singla
 * Roll Number: 2401CS18
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

int yylex(void);
void yyerror(const char *s);

int line_num = 1;
int temp_count = 0;
int label_count = 0;
int has_error = 0;

/* Buffer TAC output so partial TAC is not printed if a syntax error occurs */
#define MAX_TAC_LINES 10000
char *tac_buffer[MAX_TAC_LINES];
int tac_index = 0;

void emit(const char *fmt, ...) {
    if (has_error) return;
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (tac_index < MAX_TAC_LINES) {
        tac_buffer[tac_index++] = strdup(buf);
    }
}

char* new_temp() {
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", ++temp_count);
    return strdup(buf);
}

int new_label() {
    return ++label_count;
}

typedef struct {
    int l_true;
    int l_false;
    int l_end;
} CondLabels;
%}

%union {
    char *str;
    void *ptr;
}

%token <str> ID NUM
%token IF ELSE
%token LE GE EQ NE

/* Operator Precedence and Associativity */
%left '+' '-'
%left '*' '/'
%right UMINUS

/* Resolve Dangling-Else Ambiguity (binds else to nearest unmatched if) */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type <str> expr rel_expr
%type <ptr> if_prefix

%%

program:
    stmt_list {
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

assign_stmt:
    ID '=' expr ';' {
        emit("%s = %s", $1, $3);
        free($1);
        free($3);
    }
    ;

block:
    '{' stmt_list '}'
    ;

if_prefix:
    IF '(' rel_expr ')' {
        CondLabels *lbl = (CondLabels *)malloc(sizeof(CondLabels));
        lbl->l_true = new_label();
        lbl->l_false = new_label();
        lbl->l_end = 0;
        emit("if %s goto L%d", $3, lbl->l_true);
        emit("goto L%d", lbl->l_false);
        emit("L%d:", lbl->l_true);
        free($3);
        $$ = (void *)lbl;
    }
    ;

if_stmt:
    if_prefix stmt %prec LOWER_THAN_ELSE {
        CondLabels *lbl = (CondLabels *)$1;
        emit("L%d:", lbl->l_false);
        free(lbl);
    }
    | if_prefix stmt ELSE {
        CondLabels *lbl = (CondLabels *)$1;
        lbl->l_end = new_label();
        emit("goto L%d", lbl->l_end);
        emit("L%d:", lbl->l_false);
    } stmt {
        CondLabels *lbl = (CondLabels *)$1;
        emit("L%d:", lbl->l_end);
        free(lbl);
    }
    ;

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

expr:
    expr '+' expr {
        char *t = new_temp();
        emit("%s = %s + %s", t, $1, $3);
        $$ = t;
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
        $$ = $2;
    }
    | ID {
        $$ = $1;
    }
    | NUM {
        $$ = $1;
    }
    ;

%%

int yylex(void) {
    int c;
    while ((c = getchar()) != EOF) {
        if (c == ' ' || c == '\t' || c == '\r') {
            continue;
        }
        if (c == '\n') {
            line_num++;
            continue;
        }
        /* Handle single-line and multi-line comments */
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
            if (c != EOF) ungetc(c, stdin);
            
            if (strcmp(buf, "if") == 0) return IF;
            if (strcmp(buf, "else") == 0) return ELSE;
            
            yylval.str = strdup(buf);
            return ID;
        }
        /* Numbers */
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
        /* Two-character Relational Operators */
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
        return c;
    }
    return 0;
}

void yyerror(const char *s) {
    (void)s;
    has_error = 1;
    printf("Syntax error at line %d\n", line_num);
}

int main(void) {
    if (yyparse() != 0 || has_error) {
        return 1;
    }
    return 0;
}