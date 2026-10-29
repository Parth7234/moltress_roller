#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo "Compiling the Lex Analyzer..."
flex lexer.l
gcc lex.yy.c -o lexer

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed.${NC}"
    exit 1
fi

total_tests=0
passed_tests=0

run_test() {
    local test_name="$1"
    local input_text="$2"
    local expected_output="$3"
    
    total_tests=$((total_tests + 1))
    echo -e "$input_text" > temp_input.c
    echo -e "$expected_output" > temp_expected.txt
    
    ./lexer < temp_input.c > temp_actual.txt
    
    if diff -w temp_expected.txt temp_actual.txt > /dev/null; then
        echo -e "${GREEN}[PASS]${NC} $test_name"
        passed_tests=$((passed_tests + 1))
    else
        echo -e "${RED}[FAIL]${NC} $test_name"
        cat temp_actual.txt
    fi
    rm -f temp_input.c temp_expected.txt temp_actual.txt
}

# CASES 1, 2, 3, 5, 7, 11: Standard constructs together
input_1="#define MAX 100
// Single comment
\"Hello World\"
'A'
/* Multi
Line */
count"
expected_1="PREPROCESSOR       #define MAX 100
STRING LITERAL     \"Hello World\"
CHAR LITERAL       'A'
IDENTIFIER         count

Source Code Statistics

Lines			: 8
Comments		: 2
String literals		: 1
Character literals	: 1
Preprocessor		: 1
Identifiers		: 1

ACCEPT"
run_test "Test 1: Standard Constructs Together" "$input_1" "$expected_1"

# CASES 4 & 6: Comment-like chars in strings and Escaped chars
input_2="\"// This is not a comment\"
\"/* Nor is this */\"
'\\n'
'\\t'"
expected_2="STRING LITERAL     \"// This is not a comment\"
STRING LITERAL     \"/* Nor is this */\"
CHAR LITERAL       '\\n'
CHAR LITERAL       '\\t'

Source Code Statistics

Lines			: 4
Comments		: 0
String literals		: 2
Character literals	: 2
Preprocessor		: 0
Identifiers		: 0

ACCEPT"
run_test "Test 2: Edge Cases (Cases 4 & 6)" "$input_2" "$expected_2"

# CASE 8: Unterminated string
input_3="\"This string has no end
count"
expected_3="
Source Code Statistics

Lines			: 1
Comments		: 0
String literals		: 0
Character literals	: 0
Preprocessor		: 0
Identifiers		: 0

REJECT"
run_test "Test 3: Unterminated String (Case 8)" "$input_3" "$expected_3"

# CASE 10: Unterminated multi-line comment
input_4="/* This comment never ends
count = 1;"
expected_4="
Source Code Statistics

Lines			: 3
Comments		: 0
String literals		: 0
Character literals	: 0
Preprocessor		: 0
Identifiers		: 0

REJECT"
run_test "Test 4: Unterminated Comment (Case 10)" "$input_4" "$expected_4"

# CASE 9: Unterminated character literal
input_5="'c
count"
expected_5="
Source Code Statistics

Lines			: 1
Comments		: 0
String literals		: 0
Character literals	: 0
Preprocessor		: 0
Identifiers		: 0

REJECT"
run_test "Test 5: Unterminated Char (Case 9)" "$input_5" "$expected_5"

echo -e "----------------------------------------"
if [ "$passed_tests" -eq "$total_tests" ]; then
    echo -e "${GREEN}All $total_tests tests passed successfully!${NC}"
else
    echo -e "${RED}$passed_tests out of $total_tests tests passed.${NC}"
fi

rm -f lex.yy.c lexer