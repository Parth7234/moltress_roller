// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LEN 1000

// Helper Stack for Operators (Phase 1)
char opStack[MAX_LEN];
int opTop = -1;

void pushOp(char op) { opStack[++opTop] = op; }
char popOp() { return opTop == -1 ? '\0' : opStack[opTop--]; }
char peekOp() { return opTop == -1 ? '\0' : opStack[opTop]; }
int isOpStackEmpty() { return opTop == -1; }

// --- Helper Stack for Evaluation (Phase 2) ---
int valStack[MAX_LEN];
int valTop = -1;

void pushVal(int val) { valStack[++valTop] = val; }
int popVal(int* err) { 
    if (valTop == -1) { *err = 1; return 0; }
    return valStack[valTop--]; 
}

// --- Precedence Checker ---
int precedence(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

// --- Phase 1: Infix to Postfix (Machine-Independent) ---
int toPostfix(const char* infix, char* postfix) {
    int i = 0, j = 0;
    opTop = -1; // Reset stack for safety
    
    while (infix[i] != '\0') {
        // Skip spaces
        if (isspace(infix[i])) {
            i++;
            continue;
        }
        
        // Tokenize multi-digit numbers
        if (isdigit(infix[i])) {
            while (isdigit(infix[i])) {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' '; // Add space separator for evaluating later
        } 
        else if (infix[i] == '(') {
            pushOp(infix[i]);
            i++;
        } 
        else if (infix[i] == ')') {
            while (!isOpStackEmpty() && peekOp() != '(') {
                postfix[j++] = popOp();
                postfix[j++] = ' ';
            }
            if (isOpStackEmpty()) {
                printf("Error: Mismatched parentheses.\n");
                return 0;
            }
            popOp(); // Discard the '('
            i++;
        } 
        else if (infix[i] == '+' || infix[i] == '-' || infix[i] == '*' || infix[i] == '/') {
            while (!isOpStackEmpty() && peekOp() != '(' && precedence(peekOp()) >= precedence(infix[i])) {
                postfix[j++] = popOp();
                postfix[j++] = ' ';
            }
            pushOp(infix[i]);
            i++;
        } 
        else {
            printf("Error: Invalid character '%c'.\n", infix[i]);
            return 0;
        }
    }
    
    // Pop any remaining operators
    while (!isOpStackEmpty()) {
        if (peekOp() == '(') {
            printf("Error: Mismatched parentheses.\n");
            return 0;
        }
        postfix[j++] = popOp();
        postfix[j++] = ' ';
    }
    
    // Null terminate the postfix string
    postfix[j] = '\0';
    return 1;
}

// --- Phase 2: Evaluate Postfix (Machine-Dependent) ---
int evalPostfix(const char* postfix, int* result) {
    int i = 0;
    valTop = -1; // Reset stack for safety
    int err = 0;
    
    while (postfix[i] != '\0') {
        // Skip spaces
        if (isspace(postfix[i])) {
            i++;
            continue;
        }
        
        // Reconstruct multi-digit numbers from tokens
        if (isdigit(postfix[i])) {
            int num = 0;
            while (isdigit(postfix[i])) {
                num = num * 10 + (postfix[i] - '0');
                i++;
            }
            pushVal(num);
        } 
        else if (postfix[i] == '+' || postfix[i] == '-' || postfix[i] == '*' || postfix[i] == '/') {
            int val2 = popVal(&err); // Right operand
            int val1 = popVal(&err); // Left operand
            if (err) {
                printf("Error: Invalid postfix expression.\n");
                return 0;
            }
            
            switch (postfix[i]) {
                case '+': pushVal(val1 + val2); break;
                case '-': pushVal(val1 - val2); break;
                case '*': pushVal(val1 * val2); break;
                case '/': 
                    if (val2 == 0) {
                        printf("Error: Division by zero.\n");
                        return 0;
                    }
                    pushVal(val1 / val2); 
                    break;
            }
            i++;
        }
    }
    
    *result = popVal(&err);
    // If there was a popping error, or if extra values remain on the stack
    if (err || valTop != -1) {
        printf("Error: Invalid postfix expression.\n");
        return 0;
    }
    
    return 1;
}

// --- Driver Program ---
int main() {
    char infix[MAX_LEN];
    char postfix[MAX_LEN];
    int result;
    
    printf("Enter expression: ");
    if (!fgets(infix, sizeof(infix), stdin)) return 0;
    
    // Remove the trailing newline read by fgets
    infix[strcspn(infix, "\n")] = 0;
    
    // Execute Phase 1
    if (toPostfix(infix, postfix)) {
        printf("Postfix:\n%s\n", postfix);
        
        // Execute Phase 2
        if (evalPostfix(postfix, &result)) {
            printf("Result:\n%d\n", result);
        }
    }
    
    return 0;
}

update thsi code so that it doesnot look ai genenrated and can be submitted fo rhte alb assignment