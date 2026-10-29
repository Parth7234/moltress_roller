// Compile: gcc Assignment1.c -o Assignment1

// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Global variables to maintain the state of our parser
const char *expr;
int pos = 0;
int error_flag = 0;

// Helper function to safely ignore any number of spaces between operators and operands[cite: 43].
void skip_spaces() {
    while (expr[pos] == ' ' || expr[pos] == '\t' || expr[pos] == '\n') {
        pos++;
    }
}

// Function prototypes for our mutually recursive descent parser[cite: 43].
long long evalsum();
long long evalterm();
long long evalfactor();

// evalsum() handles the addition operator (+).
// It assumes the expression is a sum of one or more terms, identifying the outermost '+' signs[cite: 43].
long long evalsum() {
    skip_spaces();
    // An expression must start with at least one term
    long long val = evalterm();
    
    skip_spaces();
    // Locate the outermost '+' signs linking multiple terms[cite: 43]
    while (expr[pos] == '+') {
        pos++; // Consume the '+'
        long long next_val = evalterm(); // Parse the next term
        val += next_val;
        skip_spaces();
    }
    return val;
}

// evalterm() handles the multiplication operator (*).
// It is called for each summand and attempts to find the outermost '*' signs within a single term[cite: 43].
long long evalterm() {
    skip_spaces();
    // A term must start with at least one factor (a number or parenthesized expression)
    long long val = evalfactor();
    
    skip_spaces();
    // Locate the '*' signs linking multiple factors
    while (expr[pos] == '*') {
        pos++; // Consume the '*'
        long long next_val = evalfactor(); // Parse the next factor
        val *= next_val;
        skip_spaces();
    }
    return val;
}

// evalfactor() extracts the deepest operands: either a multi-digit integer or a parenthesized expression[cite: 43].
long long evalfactor() {
    skip_spaces();
    
    // Handle parentheses by mutually recursing back to evalsum()
    if (expr[pos] == '(') {
        pos++; // Consume '('
        long long val = evalsum();
        skip_spaces();
        if (expr[pos] == ')') {
            pos++; // Consume ')'
        } else {
            // If we reach here without a closing parenthesis, the expression is invalid[cite: 43].
            error_flag = 1; 
        }
        return val;
    } 
    // Handle positive integer operands[cite: 43]
    else if (isdigit(expr[pos])) {
        long long val = 0;
        // Construct integers that contain multiple digits character by character[cite: 43].
        while (isdigit(expr[pos])) {
            val = val * 10 + (expr[pos] - '0');
            pos++;
        }
        return val;
    } 
    else {
        // Missing operand or invalid character encountered (e.g., '-', '/', or letters)[cite: 43]
        error_flag = 1;
        return 0;
    }
}

int main() {
    char buffer[10000];
    
    // The user enters an expression that is read as a string at the beginning of the program[cite: 43].
    if (!fgets(buffer, sizeof(buffer), stdin)) return 0;
    
    expr = buffer;
    pos = 0;
    error_flag = 0;
    
    // Begin parsing from the highest-level grammar rule (addition/sum)
    long long result = evalsum();
    
    skip_spaces();
    
    // After parsing a valid expression, we should be exactly at the end of the string.
    // Leftover characters (e.g., "12 + 4 extra") indicate an invalid arithmetic expression[cite: 43].
    if (expr[pos] != '\0') {
        error_flag = 1;
    }
    
    // Print the final numeric value or trigger an error if the user supplied an invalid arithmetic expression[cite: 43].
    if (error_flag) {
        printf("Error: Invalid arithmetic expression.\n");
    } else {
        printf("%lld\n", result);
    }
    
    return 0;
}