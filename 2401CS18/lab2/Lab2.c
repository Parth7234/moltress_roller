// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 1005

// global stacks 
char op_st[MAX];
int top_op = -1;

int val_st[MAX];
int top_val = -1;

// simple precedence check
int prec(char c) {
    if (c == '*' || c == '/') return 2;
    if (c == '+' || c == '-') return 1;
    return 0;
}

// Phase 1 – toPostfix(): implement the shunting-yard algorithm 
// with an operator stack; respect precedence and left-associativity.
int toPostfix(char* s, char* res) {
    int i = 0, k = 0;
    top_op = -1; 
    
    // Tokenizer – split the input string into numbers, operators, and parentheses
    while (s[i]) {
        // Tokenizer: spaces are optional between tokens
        if (s[i] == ' ' || s[i] == '\t') {
            i++;
            continue;
        }
        
        // Tokenizer: numbers may have multiple digits
        if (isdigit(s[i])) {
            while (isdigit(s[i])) {
                res[k++] = s[i++];
            }
            res[k++] = ' ';
        } 
        else if (s[i] == '(') {
            op_st[++top_op] = s[i++];
        } 
        else if (s[i] == ')') {
            while (top_op != -1 && op_st[top_op] != '(') {
                res[k++] = op_st[top_op--];
                res[k++] = ' ';
            }
            // Error detection – mismatched parentheses 
            if (top_op == -1) {
                printf("Error: Mismatched parentheses.\n");
                return 0;
            }
            top_op--; // discard the '('
            i++;
        } 
        // Precedence logic ensures *, / are higher than +, - and handles left-associativity
        else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
            while (top_op != -1 && op_st[top_op] != '(' && prec(op_st[top_op]) >= prec(s[i])) {
                res[k++] = op_st[top_op--];
                res[k++] = ' ';
            }
            op_st[++top_op] = s[i++];
        } 
        else {
            // Error detection – invalid characters produce an error message
            printf("Error: Invalid character '%c'.\n", s[i]);
            return 0;
        }
    }
    
    // flush remaining operators
    while (top_op != -1) {
        // Error detection – leftover open mismatched parentheses
        if (op_st[top_op] == '(') {
            printf("Error: Mismatched parentheses.\n");
            return 0;
        }
        res[k++] = op_st[top_op--];
        res[k++] = ' ';
    }
    res[k] = '\0';
    return 1;
}

// Phase 2 – evalPostfix(): evaluate the postfix token list with a numeric stack
int evalPostfix(char* exp, int* ans) {
    int i = 0;
    top_val = -1;
    int error_flag = 0;
    
    while (exp[i]) {
        if (exp[i] == ' ') {
            i++;
            continue;
        }
        
        if (isdigit(exp[i])) {
            int num = 0;
            while (isdigit(exp[i])) {
                num = num * 10 + (exp[i] - '0');
                i++;
            }
            val_st[++top_val] = num;
        } 
        else {
            // need at least two operands to apply an operator
            if (top_val < 1) {
                error_flag = 1;
                break;
            }
            
            int v2 = val_st[top_val--];
            int v1 = val_st[top_val--];
            
            if (exp[i] == '+') val_st[++top_val] = v1 + v2;
            else if (exp[i] == '-') val_st[++top_val] = v1 - v2;
            else if (exp[i] == '*') val_st[++top_val] = v1 * v2;
            else if (exp[i] == '/') {
                // Error Detection Phase 2 – detect division by zero
                if (v2 == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                val_st[++top_val] = v1 / v2;
            }
            i++;
        }
    }
    
    if (error_flag || top_val != 0) {
        printf("Error: Invalid postfix expression.\n");
        return 0;
    }
    
    *ans = val_st[top_val--];
    return 1;
}

// Driver – print the postfix form and then the final value, on separate lines
int main() {
    char s[MAX];
    char p[MAX];
    int ans;
    
    printf("Enter expression: ");
    if (!fgets(s, MAX, stdin)) return 0;
    
    // strip newline character from fgets
    s[strcspn(s, "\n")] = 0;
    
    if (toPostfix(s, p)) {
        printf("Postfix:\n%s\n", p);
        if (evalPostfix(p, &ans)) {
            printf("Result:\n%d\n", ans);
        }
    }
    
    return 0;
}