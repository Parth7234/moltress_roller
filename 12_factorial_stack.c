#include <stdio.h>
#include <stdlib.h>

// Stack structure to store state of factorial calculation
struct stack {
    int *arr;     // Array to store state (current value of n)
    int top;      // Stack pointer
    int size;     // Size of the stack
};

// Function to initialize the stack
void initStack(struct stack *s, int size) {
    s->arr = (int *)malloc(size * sizeof(int));
    s->top = -1;
    s->size = size;
}

// Function to check if the stack is empty
int isEmpty(struct stack *s) {
    return s->top == -1;
}

// Function to push an element to the stack
void push(struct stack *s, int value) {
    if (s->top < s->size - 1) {
        s->arr[++(s->top)] = value;
    }
}

// Function to pop an element from the stack
int pop(struct stack *s) {
    if (!isEmpty(s)) {
        return s->arr[(s->top)--];
    }
    return -1; // Stack underflow
}

// Function to simulate the factorial calculation using stack
int factorialUsingStack(int n) {
    struct stack s;
    initStack(&s, n); // Allocate a stack with size n

    int result = 1;

    // Push all values from n down to 1 to simulate the recursive calls
    while (n > 1) {
        push(&s, n);
        n--;
    }

    // Now, process the stack to simulate returning from recursive calls
    while (!isEmpty(&s)) {
        result *= pop(&s);
    }

    return result;
}

int main() {
    int n;
    printf("Enter a number to calculate factorial: ");
    scanf("%d", &n);

    int result = factorialUsingStack(n);
    printf("Factorial of %d is: %d\n", n, result);

    return 0;
}
