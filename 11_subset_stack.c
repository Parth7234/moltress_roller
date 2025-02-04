#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 100 // Maximum size of character array

// Stack structure to simulate recursion
typedef struct {
    int index;
    char subset[MAX_N];
    int subsetSize;
} StackFrame;

// Recursive function to generate subsets of a character array
void generateCharSubsetsRecursive(char arr[], int n, int index, char subset[], int subsetSize) {
    if (index == n) { // Base case: print subset
        printf("{ ");
        for (int i = 0; i < subsetSize; i++) {
            printf("%c ", subset[i]);
        }
        printf("}\n");
        return;
    }

    // Include current character
    subset[subsetSize] = arr[index];
    generateCharSubsetsRecursive(arr, n, index + 1, subset, subsetSize + 1);

    // Exclude current character
    generateCharSubsetsRecursive(arr, n, index + 1, subset, subsetSize);
}

// Iterative function to simulate recursion using an explicit stack
void generateCharSubsetsIterative(char arr[], int n) {
    StackFrame stack[MAX_N * MAX_N]; // Stack to simulate function calls
    int top = -1;

    // Initial state: index = 0, empty subset
    StackFrame initial = {0, "", 0};
    stack[++top] = initial;

    while (top >= 0) {
        StackFrame current = stack[top--];

        // If we reached the end of the array, print the subset
        if (current.index == n) {
            printf("{ ");
            for (int i = 0; i < current.subsetSize; i++) {
                printf("%c ", current.subset[i]);
            }
            printf("}\n");
            continue;
        }

        // Create a copy of the current state (without modifying original)
        StackFrame exclude = current;
        exclude.index++;

        // Include case: add current character and push to stack
        StackFrame include = current;
        include.subset[include.subsetSize++] = arr[current.index];
        include.index++;

        // Push "exclude" first, then "include" to maintain order (LIFO)
        stack[++top] = exclude;
        stack[++top] = include;
    }
}

int main() {
    int n;
    printf("Enter the number of characters in the array: ");
    scanf("%d", &n);

    char arr[MAX_N], subset[MAX_N];
    printf("Enter %d characters (without spaces): ", n);
    for (int i = 0; i < n; i++) {
        scanf(" %c", &arr[i]); // Space before %c to avoid newline issues
    }

    printf("\nSubsets using Recursion:\n");
    generateCharSubsetsRecursive(arr, n, 0, subset, 0);

    printf("\nSubsets using Stack Simulation (Iterative):\n");
    generateCharSubsetsIterative(arr, n);

    return 0;
}
