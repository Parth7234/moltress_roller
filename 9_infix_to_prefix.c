#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Stack structure
struct stack {
    int size;
    int top;
    char *arr;
};

// Check if stack is empty
int isEmpty(struct stack *ptr) {
    return ptr->top == -1;
}

// Check if stack is full
int isFull(struct stack *ptr) {
    return ptr->top == ptr->size - 1;
}

// Push element onto stack
void push(struct stack *ptr, char val) {
    if (isFull(ptr)) {
        printf("Stack Overflow! Cannot push %c to the stack\n", val);
    } else {
        ptr->arr[++(ptr->top)] = val;
    }
}

// Pop element from stack
char pop(struct stack *ptr) {
    if (isEmpty(ptr)) {
        printf("Stack Underflow! Cannot pop from the stack\n");
        return -1;
    } else {
        return ptr->arr[(ptr->top)--];
    }
}

// Get top element of stack
char stackTop(struct stack* sp) {
    if (!isEmpty(sp))
        return sp->arr[sp->top];
    return '\0';
}

// Check precedence of operators
int precedence(char ch) {
    if (ch == '*' || ch == '/')
        return 3;
    else if (ch == '+' || ch == '-')
        return 2;
    else
        return 0;
}

// Check if character is an operator
int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}

// Reverse a string
void reverseString(char *str) {
    int length = strlen(str);
    int start = 0, end = length - 1;
    while (start < end) {
        // Swap characters
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Convert infix expression to prefix
char* infixToPrefix(char* infix) {
    int len = strlen(infix);

    // Step 1: Reverse the infix expression
    reverseString(infix);

    // Step 2: Swap '(' with ')' and vice versa
    for (int i = 0; i < len; i++) {
        if (infix[i] == '(')
            infix[i] = ')';
        else if (infix[i] == ')')
            infix[i] = '(';
    }

    // Step 3: Convert reversed infix to postfix
    struct stack *sp = (struct stack *) malloc(sizeof(struct stack));
    sp->size = len; 
    sp->top = -1;
    sp->arr = (char *) malloc(sp->size * sizeof(char));

    char *postfix = (char *) malloc((len + 1) * sizeof(char));
    int i = 0, j = 0;

    while (infix[i] != '\0') {
        if (!isOperator(infix[i]) && infix[i] != '(' && infix[i] != ')') {
            postfix[j++] = infix[i++];
        } else if (infix[i] == '(') {
            push(sp, infix[i++]);
        } else if (infix[i] == ')') {
            while (!isEmpty(sp) && stackTop(sp) != '(') {
                postfix[j++] = pop(sp);
            }
            pop(sp); // Pop '('
            i++;
        } else {
            while (!isEmpty(sp) && precedence(infix[i]) <= precedence(stackTop(sp))) {
                postfix[j++] = pop(sp);
            }
            push(sp, infix[i++]);
        }
    }

    while (!isEmpty(sp)) {
        postfix[j++] = pop(sp);
    }
    postfix[j] = '\0';

    // Step 4: Reverse postfix to get prefix
    reverseString(postfix);
    
    return postfix;
}

int main() {
    char infix[100];
    printf("Enter an infix expression: ");
    scanf("%s", infix); // Read infix expression from user

    char* prefix = infixToPrefix(infix);
    printf("Prefix expression: %s\n", prefix);

    free(prefix);
    return 0;
}
