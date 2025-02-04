#include <stdio.h>
#include <stdlib.h>

#define MAX_N 10 // Define max size for array

// Stack structure to store the state of permutations
typedef struct {
    int perm[MAX_N]; // Current permutation
    int used[MAX_N]; // Array to mark used elements
    int depth;       // Current depth in permutation
} StackFrame;

typedef struct {
    StackFrame data[MAX_N * MAX_N]; // Stack storage
    int top;
} Stack;

// Initialize stack
void initStack(Stack *s) {
    s->top = -1;
}

// Push state onto stack
void push(Stack *s, StackFrame frame) {
    if (s->top < MAX_N * MAX_N - 1) {
        s->data[++(s->top)] = frame;
    }
}

// Pop state from stack
StackFrame pop(Stack *s) {
    return s->data[(s->top)--];
}

// Check if stack is empty
int isEmpty(Stack *s) {
    return s->top == -1;
}

// Function to generate permutations using stack
void generatePermutations(int arr[], int n) {
    Stack s;
    initStack(&s);

    // Initial empty permutation
    StackFrame initial = {{0}, {0}, 0};
    push(&s, initial);

    while (!isEmpty(&s)) {
        StackFrame current = pop(&s);

        // If we have used all elements, print the permutation
        if (current.depth == n) {
            printf("{ ");
            for (int i = 0; i < n; i++) {
                printf("%d ", current.perm[i]);
            }
            printf("}\n");
            continue;
        }

        // Try adding each unused element
        for (int i = 0; i < n; i++) {
            if (!current.used[i]) {
                StackFrame next = current;
                next.perm[next.depth] = arr[i];
                next.used[i] = 1;
                next.depth++;
                push(&s, next);
            }
        }
    }
}

int main() {
    int n;
    printf("Enter the number of elements: ");
    scanf("%d", &n);

    int arr[MAX_N];
    printf("Enter %d elements: ", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    printf("\nAll Permutations:\n");
    generatePermutations(arr, n);

    return 0;
}
