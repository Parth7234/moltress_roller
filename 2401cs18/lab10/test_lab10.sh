#!/bin/bash

# Ensure the executable exists
if [ ! -f "./lab10" ]; then
    echo "Executable ./lab10 not found! Compiling..."
    bison -d Lab10.y
    gcc Lab10.tab.c -o lab10
    if [ $? -ne 0 ]; then
        echo "Compilation failed!"
        exit 1
    fi
fi

TOTAL_TESTS=0
PASSED_TESTS=0

# Helper function to run a test
run_test() {
    local test_name="$1"
    local input_code="$2"
    local expected_output="$3"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    # Write the input to input.c (which lab10 reads)
    echo "$input_code" > input.c

    # Run the compiler and capture output
    ./lab10 > actual_output.txt

    # Compare actual output with expected output ignoring trailing whitespaces/blank lines
    diff -Z -B <(echo "$expected_output") actual_output.txt > diff_output.txt

    if [ -s diff_output.txt ]; then
        echo "❌ Test $TOTAL_TESTS FAILED: $test_name"
        echo "--- Expected ---"
        echo "$expected_output"
        echo "--- Actual ---"
        cat actual_output.txt
        echo "----------------"
    else
        echo "✅ Test $TOTAL_TESTS PASSED: $test_name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    fi
}

echo "Starting Lab 10 Tests..."
echo "========================"

# Test 1: Simple assignment
run_test "Simple Assignment" \
"a = 10;" \
"a = 10"

# Test 2: Operator precedence (Multiplication before addition)
run_test "Operator Precedence" \
"a = b + c * d;" \
"t1 = c * d
t2 = b + t1
a = t2"

# Test 3: Parentheses altering precedence
run_test "Parentheses" \
"x = (a + b) * c;" \
"t1 = a + b
t2 = t1 * c
x = t2"

# Test 4: Unary minus and relational operators
run_test "Unary Minus & Condition" \
"if (-a < b) { x = 1; }" \
"t1 = -a
if t1 < b goto L1
goto L2
L1:
x = 1
L2:"

# Test 5: If-else statement
run_test "If-Else Statement" \
"if (a > b) {
    x = a;
} else {
    x = b;
}" \
"if a > b goto L1
goto L2
L1:
x = a
goto L3
L2:
x = b
L3:"

# Test 6: Nested blocks and multiple statements
run_test "Nested Blocks" \
"{
    a = 10;
    b = 20;
    if (a < b) { c = a + b; }
}" \
"a = 10
b = 20
if a < b goto L1
goto L2
L1:
t1 = a + b
c = t1
L2:"

# Test 7: Dangling-Else Resolution (else binds to innermost if)
run_test "Dangling-Else Resolution" \
"if (a > b)
    if (c > d)
        x = 1;
    else
        x = 2;" \
"if a > b goto L1
goto L2
L1:
if c > d goto L3
goto L4
L3:
x = 1
goto L5
L4:
x = 2
L5:
L2:"

# Test 8: Syntax Error - Missing semicolon
run_test "Syntax Error - Missing semicolon" \
"a = b + " \
"Syntax error at line 1"

# Test 9: Syntax Error with Line Tracking
run_test "Syntax Error - Line Tracking" \
"a = 10;
b = 20;

c = a * ;" \
"Syntax error at line 4"

# Test 10: Comments and Whitespace Handling
run_test "Comments and Whitespaces" \
"// This is a comment
a = 10; /* Inline comment */
    b = 20; 
/* Multi-line
   comment */
c = a + b;" \
"a = 10
b = 20
t1 = a + b
c = t1"

echo "========================"
echo "Final Score: $PASSED_TESTS / $TOTAL_TESTS test cases passed."

# Cleanup temporary files
rm -f input.c actual_output.txt diff_output.txt