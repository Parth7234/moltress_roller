#!/bin/bash

# Compile the lexer strictly according to assignment rules
echo "Compiling lexer.c..."
gcc lexer.c -o lexer
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi
echo "Compilation successful."
echo "------------------------------------------------"

# Helper function to run a test
run_test() {
    local test_name="$1"
    local input_string="$2"
    
    echo "### TEST: $test_name ###"
    echo "Input:"
    echo "$input_string"
    echo -e "\nOutput:"
    
    # Create temp file, run lexer, and clean up
    echo "$input_string" > temp_test.txt
    ./lexer < temp_test.txt
    rm temp_test.txt
    
    echo "------------------------------------------------"
}

# Test 1: Keyword Prefix (Longest Match Rule)
# "ifx" must be IDENTIFIER ifx, not KEYWORD if followed by IDENTIFIER x
run_test "Keyword Prefix (Longest Match)" "ifx while_loop returnVal string2"

# Test 2: Integer vs. Real Numbers
# 25 -> INTEGER, 25.50 -> REAL
run_test "Integer vs Real Constants" "25 25.50 0 0.99"

# Test 3: Relational Operators (Longest Match Rule)
# "<" followed by "<=" should not break unexpectedly. 
run_test "Multi-character Operators" "< <= > >= == !="

# Test 4: Tricky Operator Adjacency
# Tests if the lexer handles back-to-back operators correctly based on longest match
run_test "Operator Adjacency" "<<<= =<<"

# Test 5: Whitespace Handling
# Multiple spaces, tabs, and newlines should be ignored and produce no tokens
run_test "Whitespace Handling" "int    x 
= 
	10 ;"

# Test 6: Lexical Error (REJECT)
# Should stop scanning and print the exact unexpected character
run_test "Lexical Error Handling" "int x = 10 @ 20;"

echo "Testing complete!"