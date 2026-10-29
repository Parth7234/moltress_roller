// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 1005

// Global stacks are used here to simulate memory allocation for our parsing and evaluation phases.
// op_st: Operator stack used purely in Phase 1 (Shunting-yard) to temporarily hold operators and parentheses.
char op_st[MAX];
int top_op = -1;

// val_st: Value stack used purely in Phase 2 (Evaluation) to hold numbers (operands) as we compute the result.
int val_st[MAX];
int top_val = -1;

// Helper function to define the strict hierarchy of operator precedence.
// Multiplication and division happen before addition and subtraction.
int prec(char c) {
    if (c == '*' || c == '/') return 2;
    if (c == '+' || c == '-') return 1;
    return 0; // Parentheses or invalid characters get lowest precedence
}

// Phase 1 – toPostfix(): The Front End.
// Converts human-readable Infix (e.g., 3 + 4) to machine-friendly Postfix (e.g., 3 4 +).
// It uses the Shunting-Yard algorithm to reorder tokens based on precedence without evaluating anything.
int toPostfix(char* s, char* res) {
    int i = 0, k = 0; // i traverses the input string 's', k builds the output string 'res'
    top_op = -1; 
    
    while (s[i]) {
        // Skip whitespace so the parser isn't confused by formatting.
        if (s[i] == ' ' || s[i] == '\t') {
            i++;
            continue;
        }
        
        // Tokenizer: If we hit a digit, it might be a multi-digit number (e.g., '1' followed by '2' = 12).
        // We loop to extract the full number and append it directly to our output (Postfix) string.
        if (isdigit(s[i])) {
            while (isdigit(s[i])) {
                res[k++] = s[i++];
            }
            res[k++] = ' '; // Add a space delimiter after the number so Phase 2 can tell where it ends
        } 
        // Left parenthesis creates a new sub-expression context. We push it to the stack as a boundary marker.
        else if (s[i] == '(') {
            op_st[++top_op] = s[i++];
        } 
        // Right parenthesis signals the end of a sub-expression.
        // We pop everything off the stack to the output until we find the matching left parenthesis.
        else if (s[i] == ')') {
            while (top_op != -1 && op_st[top_op] != '(') {
                res[k++] = op_st[top_op--];
                res[k++] = ' ';
            }
            // If we empty the stack without finding a '(', the user typed something like "3 + 4 )"
            if (top_op == -1) {
                printf("Error: Mismatched parentheses.\n");
                return 0;
            }
            top_op--; // Discard the '(' from the stack; it doesn't go into the postfix output.
            i++;
        } 
        // Core Shunting-Yard Logic for Operators (+, -, *, /)
        else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
            // Before pushing the current operator, we must check what is already on the stack.
            // If the stack has an operator with GREATER OR EQUAL precedence, it must be executed first 
            // to respect left-associativity and standard math rules. So, we pop it to the output string.
            while (top_op != -1 && op_st[top_op] != '(' && prec(op_st[top_op]) >= prec(s[i])) {
                res[k++] = op_st[top_op--];
                res[k++] = ' ';
            }
            // Once higher-precedence operators are cleared, safely push the current operator.
            op_st[++top_op] = s[i++];
        } 
        else {
            // Catches stray letters or symbols like 'a', '&', or '$'
            printf("Error: Invalid character '%c'.\n", s[i]);
            return 0;
        }
    }
    
    // The input string is fully read, but operators might still be waiting on the stack.
    // Flush them all to the output string.
    while (top_op != -1) {
        // If we find a '(' still hanging around, the user typed something like "( 3 + 4"
        if (op_st[top_op] == '(') {
            printf("Error: Mismatched parentheses.\n");
            return 0;
        }
        res[k++] = op_st[top_op--];
        res[k++] = ' ';
    }
    res[k] = '\0'; // Null-terminate the string so it's a valid C string.
    return 1; // Success
}

// Phase 2 – evalPostfix(): The Back End.
// Executes the postfix string using a virtual machine approach (operand stack).
int evalPostfix(char* exp, int* ans) {
    int i = 0;
    top_val = -1; // Reset value stack for evaluation
    int error_flag = 0;
    
    while (exp[i]) {
        if (exp[i] == ' ') {
            i++;
            continue;
        }
        
        // If we see a number, parse the full integer and PUSH it onto the value stack.
        if (isdigit(exp[i])) {
            int num = 0;
            while (isdigit(exp[i])) {
                num = num * 10 + (exp[i] - '0'); // Convert char to integer math
                i++;
            }
            val_st[++top_val] = num;
        } 
        // If we see an operator, we must apply it to the last two numbers we pushed.
        else {
            // Binary operators require exactly two operands. 
            // If the stack has fewer than 2 items, the postfix string is malformed (e.g., "3 +").
            if (top_val < 1) {
                error_flag = 1;
                break;
            }
            
            // POP the top two values. 
            // Note: v2 is popped first (right operand), v1 is popped second (left operand).
            // Order matters heavily for subtraction and division!
            int v2 = val_st[top_val--];
            int v1 = val_st[top_val--];
            
            // Execute the operation and PUSH the result back onto the stack to be used by future operators.
            if (exp[i] == '+') val_st[++top_val] = v1 + v2;
            else if (exp[i] == '-') val_st[++top_val] = v1 - v2;
            else if (exp[i] == '*') val_st[++top_val] = v1 * v2;
            else if (exp[i] == '/') {
                // Hardware-level crash prevention: catch division by zero before it triggers an OS fault.
                if (v2 == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }
                val_st[++top_val] = v1 / v2;
            }
            i++;
        }
    }
    
    // Once the entire string is processed, a valid expression should leave EXACTLY ONE number on the stack (the final answer).
    // If top_val != 0, it means we have leftover numbers (e.g., "3 4 5 +", which leaves 3 and 9).
    if (error_flag || top_val != 0) {
        printf("Error: Invalid postfix expression.\n");
        return 0;
    }
    
    *ans = val_st[top_val--]; // Extract the final computed answer via pointer
    return 1;
}

// Driver Code
int main() {
    char s[MAX];
    char p[MAX];
    int ans;
    
    printf("Enter expression: ");
    // Safely read input to prevent buffer overflows (unlike gets())
    if (!fgets(s, MAX, stdin)) return 0;
    
    // fgets appends a newline ('\n') when the user hits Enter. 
    // This strips it out by replacing it with a null terminator.
    s[strcspn(s, "\n")] = 0;
    
    // Run Phase 1
    if (toPostfix(s, p)) {
        printf("Postfix:\n%s\n", p);
        // If Phase 1 succeeded, run Phase 2 on the generated postfix string
        if (evalPostfix(p, &ans)) {
            printf("Result:\n%d\n", ans);
        }
    }
    
    return 0;
}