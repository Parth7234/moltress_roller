// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char st[1000];
int top = -1;

void push(char c) {
    st[++top] = c;
}

void pop() {
    if (top != -1) top--;
}

char *result;

int precedence(char c) {
    if (c == '+' || c == '-') return 1;
    if (c == '*' || c == '/') return 2;
    return 0;
}

int postfix(char *exp) {
    int k = 0; // Index for tracking the end of the result string
    int len = strlen(exp); // Calculate length once for O(N) performance

    for (int i = 0; i < len; i++) {
        if (isdigit(exp[i])) {
            result[k++] = exp[i]; // Correct C string assignment
        }
        else if (exp[i] == '(') {
            push(exp[i]);
        }
        else if (exp[i] == ')') {
            while (top != -1 && st[top] != '(') {
                result[k++] = st[top];
                pop();
            }
            if (top != -1 && st[top] == '(') {
                pop();
            }
        }
        else { // Operators (+, -, *, /)
            // Safely check if stack is empty before checking precedence of st[top]
            while (top != -1 && precedence(exp[i]) <= precedence(st[top])) {
                result[k++] = st[top];
                pop();
            }
            push(exp[i]);
        }
    }

    // Pop any remaining operators in the stack
    while (top != -1) {
        result[k++] = st[top];
        pop();
    }
    
    // Null-terminate the string
    result[k] = '\0'; 
    return 1;
}

int main() {
    char exp[1000];
    printf("Enter the expression: ");
    
    // Using scanf to read the entire line including spaces
    scanf(" %[^\n]", exp); 
    
    result = (char*)malloc(sizeof(char) * 1000);
    
    postfix(exp);
    printf("Postfix expression: %s\n", result);
    
    free(result); // Always free dynamically allocated memory
    return 0;
}