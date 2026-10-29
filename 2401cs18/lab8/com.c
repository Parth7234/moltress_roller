// Name: Parth Singla
// Roll Number: 2401CS18

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Parser State Representation[cite: 23]
// 'st' acts as the parser stack to hold grammar symbols (terminals and non-terminals).
char st[1000];
int tp = 0; // Top pointer for the stack

// Input stream to hold the arithmetic expression
char in_str[1000];
int pos = 0; // Current read position in the input string

// peek(): The Lookahead mechanism
// Skips whitespace and peeks at the upcoming token in the input stream without consuming it.
char peek() {
    while (in_str[pos] == ' ' || in_str[pos] == '\t') {
        pos++;
    }
    // To simplify stack operations, map the two-character token "id" into a single character 'i'.
    if (in_str[pos] == 'i' && in_str[pos+1] == 'd') {
        return 'i';
    }
    return in_str[pos];
}

// shift(): Consumes the lookahead token from the input and pushes it onto the stack[cite: 22].
void shift() {
    char look = peek();
    if (look == 'i') {
        printf("SHIFT: id\n");
        tp++;
        st[tp] = 'i';
        pos += 2; // Advance by 2 for "id"
    } else {
        printf("SHIFT: %c\n", look); // Added missing newline for cleaner output formatting
        tp++;
        st[tp] = look;
        pos++; // Advance by 1 for operators or parentheses
    }
}

// Helper to assign precedence levels to operators[cite: 23].
// Multiplication (*) has higher precedence than addition (+) and subtraction (-)[cite: 23].
int get_prio(char op) {
    if (op == '*') return 2;
    if (op == '+' || op == '-') return 1;
    return 0; // Lowest priority for anything else
}

// reduce(): Checks if the top of the stack matches the Right-Hand Side (RHS) of a production rule.
// If it does (and precedence rules allow it), it replaces the RHS with the Non-Terminal 'E'[cite: 23].
int reduce() {
    char look = peek();

    // Production 4: E -> id[cite: 22]
    // id is treated as an operand and reduced immediately after being shifted[cite: 23].
    if (tp >= 1 && st[tp] == 'i') {
        printf("REDUCE: id -> E\n");
        st[tp] = 'E'; // Pop 'i', push 'E'
        return 1;     // Return 1 indicating a reduction occurred
    }

    // Parentheses Priority[cite: 23]
    // Parentheses have the highest priority. If we see "( E )" on top of the stack, 
    // reduce it to "E" immediately, bypassing normal operator arithmetic rules[cite: 23].
    if (tp >= 3 && st[tp-2] == '(' && st[tp-1] == 'E' && st[tp] == ')') {
        printf("REDUCE: (E) -> E\n");
        tp -= 2;      // Pop ")", "E", "("
        st[tp] = 'E'; // Push "E"
        return 1;
    }

    // Productions 1, 2, 3: E -> E + E | E - E | E * E[cite: 22]
    // If the stack contains "E operator E", we have a potential SHIFT/REDUCE conflict[cite: 23].
    if (tp >= 3 && st[tp-2] == 'E' && 
        (st[tp-1] == '*' || st[tp-1] == '+' || st[tp-1] == '-') && 
        st[tp] == 'E') {
        
        char op = st[tp-1]; // The operator currently on the stack
        
        // CONFLICT RESOLUTION BLOCK[cite: 23]
        // Compare the stack operator against the incoming lookahead operator.
        if (look == '*' || look == '+' || look == '-') {
            int p_op = get_prio(op);
            int p_lk = get_prio(look);

            // Rule 1: If stack operator has HIGHER precedence, REDUCE (e.g., Stack has *, incoming is +)[cite: 23].
            // Rule 2: If operators have EQUAL precedence, use left-associativity and REDUCE (e.g., Stack has +, incoming is +)[cite: 23].
            // The ">=" operator elegantly handles both rules simultaneously.
            if (p_op >= p_lk) {
                printf("REDUCE: E%cE -> E\n", op);
                tp -= 2;
                st[tp] = 'E';
                return 1;
            } else {
                // Rule 3: If incoming operator has HIGHER precedence, SHIFT (e.g., Stack has +, incoming is *)[cite: 23].
                return 0; // Returning 0 forces the main loop to call shift()
            }
        } else {
            // If the lookahead is NOT an operator (e.g., it is '$' or ')'), we must reduce the current expression.
            printf("REDUCE: E%cE -> E\n", op);
            tp -= 2;
            st[tp] = 'E';
            return 1;
        }
    }

    return 0; // No valid reduction found
}

int main() {
    // Read the arithmetic expression
    if (fgets(in_str, sizeof(in_str), stdin) == NULL) {
        return 0;
    }
    
    // Strip trailing newline character
    size_t len = strlen(in_str);
    if (len > 0 && in_str[len-1] == '\n') {
        in_str[len-1] = '\0';
        len--;
    }

    // Input Formatting: Add '$' to indicate the end of the input stream[cite: 23].
    int has_eof = 0;
    for (size_t i = 0; i < len; i++) {
        if (in_str[i] == '$') {
            has_eof = 1;
            break;
        }
    }
    if (!has_eof) {
        strcat(in_str, " $");
    }

    // Stack Initialization: The stack must initially contain '$'[cite: 23].
    st[0] = '$';
    tp = 0;

    // Core Parsing Loop
    while (1) {
        // Always try to reduce first.
        int did_red = reduce();
        if (did_red) {
            continue; // If we reduced, loop again to check for chain reductions (e.g., E+E -> E, which triggers another E+E -> E)
        }

        // If no reductions are possible, look at the next token.
        char look = peek();
        if (look == '$') {
            break; // End of input reached
        } else {
            shift(); // Otherwise, shift the token onto the stack
        }
    }

    // ACCEPT/REJECT Conditions[cite: 24]
    // The string is structurally valid ONLY IF the stack has been perfectly reduced 
    // down to just the end marker '$' and the starting non-terminal 'E'[cite: 24].
    if (tp == 1 && st[0] == '$' && st[1] == 'E') {
        printf("ACCEPT\n");
    } else {
        printf("REJECT\n"); // Prints if there are leftover symbols or syntax errors[cite: 24]
    }

    return 0;
}