#!/bin/bash

# Ensure the executable exists
if [ ! -f "./lab11" ]; then
    echo "Error: ./lab11 executable not found. Please compile the parser first."
    echo "Run: bison -d Lab11.y && gcc Lab11.tab.c -o lab11"
    exit 1
fi

passed=0
total=0

run_test() {
    local description="$1"
    local input="$2"
    local expected="$3"
    
    total=$((total + 1))
    
    # Run the parser and normalize output by stripping leading/trailing whitespace
    actual=$(echo -n -e "$input" | ./lab11 2>&1 | xargs)
    expected=$(echo -n -e "$expected" | xargs)
    
    if [ "$actual" == "$expected" ]; then
        echo -e "[\033[32mPASS\033[0m] Test $total: $description"
        passed=$((passed + 1))
    else
        echo -e "[\033[31mFAIL\033[0m] Test $total: $description"
        echo "   Input:    $(echo -n -e "$input" | cat -v)"
        echo "   Expected: $expected"
        echo "   Actual:   $actual"
    fi
}

echo "================================================"
echo "    HEAVY PARSER TEST SUITE (35 TEST CASES)     "
echo "================================================"

# ---------------------------------------------------------
# CATEGORY 1: Valid Basic Statements (1 - 5)
# ---------------------------------------------------------
run_test "Simple number assignment" "a = 10;" "Input is syntactically valid."
run_test "Simple variable assignment" "x = y;" "Input is syntactically valid."
run_test "Basic addition and subtraction" "res = a + b - 5;" "Input is syntactically valid."
run_test "Basic multiplication and division" "val = x * y / 2;" "Input is syntactically valid."
run_test "Mixed operators" "total = price + tax * 2 - discount / 10;" "Input is syntactically valid."

# ---------------------------------------------------------
# CATEGORY 2: Parentheses & Precedence (6 - 10)
# ---------------------------------------------------------
run_test "Simple parentheses" "a = (b + c);" "Input is syntactically valid."
run_test "Parentheses overriding precedence" "result = (a + b) * c;" "Input is syntactically valid."
run_test "Nested parentheses" "val = ((a + b) * (c - d)) / 2;" "Input is syntactically valid."
run_test "Deeply nested parentheses (redundant)" "a = (((((10)))));" "Input is syntactically valid."
run_test "Associativity check" "a = b - c - d;" "Input is syntactically valid."

# ---------------------------------------------------------
# CATEGORY 3: Multiple Statements & Formatting (11 - 15)
# ---------------------------------------------------------
run_test "Multiple statements on one line" "a = 1; b = 2; c = a + b;" "Input is syntactically valid."
run_test "Multiple statements on multiple lines" "a = 1;\nb = 2;\nc = a * b;\n" "Input is syntactically valid."
run_test "Excessive whitespace/tabs/newlines" "\n\t a \n \t = \n \t 10 \n \t ; \n" "Input is syntactically valid."
run_test "Empty program (no input)" "" "Input is syntactically valid."
run_test "Only newlines and spaces" "   \n\n\t  \n" "Input is syntactically valid."

# ---------------------------------------------------------
# CATEGORY 4: Invalid Syntax - Assignment Errors (16 - 20)
# ---------------------------------------------------------
run_test "Missing semicolon at EOF" "a = 10" "Syntax error at line 1"
run_test "Number on Left-Hand Side (LHS)" "10 = a;" "Syntax error at line 1"
run_test "Expression on Left-Hand Side (LHS)" "a + b = 10;" "Syntax error at line 1"
run_test "Missing identifier" "= 10;" "Syntax error at line 1"
run_test "Missing assignment operator" "a 10;" "Syntax error at line 1"

# ---------------------------------------------------------
# CATEGORY 5: Invalid Syntax - Expression Errors (21 - 26)
# ---------------------------------------------------------
run_test "Dangling operator at end" "a = b + ;" "Syntax error at line 1"
run_test "Missing initial operand (unary not supported)" "a = + b;" "Syntax error at line 1"
run_test "Double operators" "a = b * / c;" "Syntax error at line 1"
run_test "Missing operator between operands" "a = b c;" "Syntax error at line 1"
run_test "Empty expression" "a = ;" "Syntax error at line 1"
run_test "Invalid character in expression" "a = b @ c;" "Syntax error at line 1"

# ---------------------------------------------------------
# CATEGORY 6: Invalid Syntax - Parentheses Errors (27 - 31)
# ---------------------------------------------------------
run_test "Missing closing parenthesis" "a = (b + c;" "Syntax error at line 1"
run_test "Missing opening parenthesis" "a = b + c);" "Syntax error at line 1"
run_test "Empty parentheses" "a = ();" "Syntax error at line 1"
run_test "Unmatched nested parenthesis" "a = ((b + c) * d;" "Syntax error at line 1"
run_test "Reversed parentheses sequence" "a = ) b + c (;" "Syntax error at line 1"

# ---------------------------------------------------------
# CATEGORY 7: Multiline Error Tracking (Line Numbers) (32 - 35)
# ---------------------------------------------------------
run_test "Error on line 2" "a = 10;\nb = ;" "Syntax error at line 2"
run_test "Error on line 3" "a = 1;\n\nc = a + ;" "Syntax error at line 3"
run_test "Missing semicolon on multi-line EOF" "a = 10;\nb = 20\n" "Syntax error at line 2"
run_test "Cascading syntax issue on line 4" "a = 1;\nb = 2;\nc = 3;\n10 = d;" "Syntax error at line 4"

echo "================================================"
echo "Test Summary: $passed / $total passed."

if [ "$passed" -eq "$total" ]; then
    echo -e "\n\033[32mSUCCESS: All tests passed flawlessly!\033[0m"
    exit 0
else
    echo -e "\n\033[31mWARNING: $(((total - passed))) tests failed. Check logs above.\033[0m"
    exit 1
fi