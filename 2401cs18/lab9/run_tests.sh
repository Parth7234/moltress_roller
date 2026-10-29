#!/bin/bash

# Automated Test Suite for CS3104 Lab Assignment 9
# Numeric Literal Analyzer and Converter Using Lex

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Detect source file name (Lab9.l or Lab9.1)
if [ -f "Lab9.l" ]; then
    SRC="Lab9.l"
elif [ -f "Lab9.1" ]; then
    SRC="Lab9.1"
else
    echo -e "${RED}Error: Neither Lab9.l nor Lab9.1 found in current directory.${NC}"
    exit 1
fi

echo "Compiling $SRC..."
lex "$SRC" && gcc lex.yy.c -o lab9
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed!${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful! Running edge-case test suite...${NC}\n"

passed=0
total=0

# Function to execute a single test case
run_test() {
    local test_name="$1"
    local input_data="$2"
    local expected_output="$3"

    total=$((total + 1))

    # Run analyzer with input
    actual_output=$(printf "%s" "$input_data" | ./lab9)

    # Compare ignoring trailing horizontal whitespace differences
    if diff -u -b -B <(printf "%s\n" "$expected_output") <(printf "%s\n" "$actual_output") > /tmp/test_diff.txt; then
        echo -e "[${GREEN}PASS${NC}] Test $total: $test_name"
        passed=$((passed + 1))
    else
        echo -e "[${RED}FAIL${NC}] Test $total: $test_name"
        echo -e "${YELLOW}--- Differences (Expected vs Actual) ---${NC}"
        cat /tmp/test_diff.txt
        echo -e "${YELLOW}----------------------------------------${NC}"
    fi
}

# ------------------------------------------------------------------------------
# Test 1: Handout Example
# ------------------------------------------------------------------------------
run_test "Handout Example" \
"int a = 42;
int mask = 0xFF; // hex mask
int perm = 0755;
int flags = 0b1010;
float big = 2.5e3;
int bad = 12abc;
" \
"1: 42 -> DECIMAL_INT (decimal value 42)
2: 0xFF -> HEX_INT (decimal value 255)
3: 0755 -> OCTAL_INT (decimal value 493)
4: 0b1010 -> BINARY_INT (decimal value 10)
5: 2.5e3 -> FLOAT (value 2500)
Lexical Error at line 6: malformed numeric literal '12abc'
--- Numeric Literal Statistics ---
Decimal integers     : 1
Octal integers       : 1
Hexadecimal integers : 1
Binary integers      : 1
Floating-point       : 1
Lexical errors       : 1
Sum of integers      : 800
Largest integer      : 493
REJECT"

# ------------------------------------------------------------------------------
# Test 2: Each Numeric Category & Valid Formats
# ------------------------------------------------------------------------------
run_test "All 5 Valid Numeric Categories" \
"2026
017
0x1a
0b1010
3.14 6.0E-2 1e+5
" \
"1: 2026 -> DECIMAL_INT (decimal value 2026)
2: 017 -> OCTAL_INT (decimal value 15)
3: 0x1a -> HEX_INT (decimal value 26)
4: 0b1010 -> BINARY_INT (decimal value 10)
5: 3.14 -> FLOAT (value 3.14)
5: 6.0E-2 -> FLOAT (value 0.06)
5: 1e+5 -> FLOAT (value 100000)
--- Numeric Literal Statistics ---
Decimal integers     : 1
Octal integers       : 1
Hexadecimal integers : 1
Binary integers      : 1
Floating-point       : 3
Lexical errors       : 0
Sum of integers      : 2077
Largest integer      : 2026
ACCEPT"

# ------------------------------------------------------------------------------
# Test 3: Boundary Zeros (0 vs 00 vs 0.0)
# ------------------------------------------------------------------------------
run_test "Zero Edge Cases (0 vs 00 vs 0.0)" \
"int x = 0;
int y = 00;
float z = 0.0;
" \
"1: 0 -> DECIMAL_INT (decimal value 0)
2: 00 -> OCTAL_INT (decimal value 0)
3: 0.0 -> FLOAT (value 0)
--- Numeric Literal Statistics ---
Decimal integers     : 1
Octal integers       : 1
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 1
Lexical errors       : 0
Sum of integers      : 0
Largest integer      : 0
ACCEPT"

# ------------------------------------------------------------------------------
# Test 4: Upper- and Lower-Case Prefixes and Exponents
# ------------------------------------------------------------------------------
run_test "Case Sensitivity in Prefixes & Exponents (0X1f, 0B11, 1E5, 2e-1)" \
"0X1f 0x1F 0B11 0b11 1E5 2e-1
" \
"1: 0X1f -> HEX_INT (decimal value 31)
1: 0x1F -> HEX_INT (decimal value 31)
1: 0B11 -> BINARY_INT (decimal value 3)
1: 0b11 -> BINARY_INT (decimal value 3)
1: 1E5 -> FLOAT (value 100000)
1: 2e-1 -> FLOAT (value 0.2)
--- Numeric Literal Statistics ---
Decimal integers     : 0
Octal integers       : 0
Hexadecimal integers : 2
Binary integers      : 2
Floating-point       : 2
Lexical errors       : 0
Sum of integers      : 68
Largest integer      : 31
ACCEPT"

# ------------------------------------------------------------------------------
# Test 5: Malformed Numeric Literals
# ------------------------------------------------------------------------------
run_test "Malformed Numeric Literals (08, 09, 0x, 0b, 0b102, 12abc, 3.14.15, 1e, 3.)" \
"08 09
0x 0b 0b102
12abc 3.14.15
1e 3. 1e+
" \
"Lexical Error at line 1: malformed numeric literal '08'
Lexical Error at line 1: malformed numeric literal '09'
Lexical Error at line 2: malformed numeric literal '0x'
Lexical Error at line 2: malformed numeric literal '0b'
Lexical Error at line 2: malformed numeric literal '0b102'
Lexical Error at line 3: malformed numeric literal '12abc'
Lexical Error at line 3: malformed numeric literal '3.14.15'
Lexical Error at line 4: malformed numeric literal '1e'
Lexical Error at line 4: malformed numeric literal '3.'
Lexical Error at line 4: malformed numeric literal '1e+'
--- Numeric Literal Statistics ---
Decimal integers     : 0
Octal integers       : 0
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 0
Lexical errors       : 10
Sum of integers      : 0
Largest integer      : 0
REJECT"

# ------------------------------------------------------------------------------
# Test 6: Out-of-Range Literals (Integer and Float)
# ------------------------------------------------------------------------------
run_test "Out-of-Range Literals (ERANGE Check)" \
"99999999999999999999
0xFFFFFFFFFFFFFFFFFFFF
1e500
7
" \
"Lexical Error at line 1: integer literal '99999999999999999999' out of range
Lexical Error at line 2: integer literal '0xFFFFFFFFFFFFFFFFFFFF' out of range
Lexical Error at line 3: float literal '1e500' out of range
4: 7 -> DECIMAL_INT (decimal value 7)
--- Numeric Literal Statistics ---
Decimal integers     : 1
Octal integers       : 0
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 0
Lexical errors       : 3
Sum of integers      : 7
Largest integer      : 7
REJECT"

# ------------------------------------------------------------------------------
# Test 7: Digits Inside Comments, Strings, Char Literals, and Identifiers
# ------------------------------------------------------------------------------
run_test "Ignoring Digits in Comments, Strings, Chars, and Identifiers" \
"// single line comment with 12345 and 0xFF
/* multi-line comment with 67890
   and 0b1010 and 3.14 */
char *s = \"port 8080 and \\\"9999\\\"\";
char c = '5';
int x1 = v0x1F + _99var + 100;
" \
"6: 100 -> DECIMAL_INT (decimal value 100)
--- Numeric Literal Statistics ---
Decimal integers     : 1
Octal integers       : 0
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 0
Lexical errors       : 0
Sum of integers      : 100
Largest integer      : 100
ACCEPT"

# ------------------------------------------------------------------------------
# Test 8: Multi-line Comment Line Counter Verification
# ------------------------------------------------------------------------------
run_test "Multi-line Comment Line Number Tracking" \
"int a = 10;
/* comment starts line 2
   line 3
   line 4
   ends line 5 */
int b = 20;
" \
"1: 10 -> DECIMAL_INT (decimal value 10)
6: 20 -> DECIMAL_INT (decimal value 20)
--- Numeric Literal Statistics ---
Decimal integers     : 2
Octal integers       : 0
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 0
Lexical errors       : 0
Sum of integers      : 30
Largest integer      : 20
ACCEPT"

# ------------------------------------------------------------------------------
# Test 9: Unterminated Multi-line Comment
# ------------------------------------------------------------------------------
run_test "Unterminated Multi-line Comment" \
"int x = 5;
/* This comment starts on line 2
   and has 12345 inside it
   but never closes
" \
"1: 5 -> DECIMAL_INT (decimal value 5)
Lexical Error at line 2: unterminated comment
--- Numeric Literal Statistics ---
Decimal integers     : 1
Octal integers       : 0
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 0
Lexical errors       : 1
Sum of integers      : 5
Largest integer      : 5
REJECT"

# ------------------------------------------------------------------------------
# Test 10: Unterminated String Literal
# ------------------------------------------------------------------------------
run_test "Unterminated String Literal" \
"char *str = \"unclosed string with 42;
int y = 9;
" \
"Lexical Error at line 1: unterminated string literal
2: 9 -> DECIMAL_INT (decimal value 9)
--- Numeric Literal Statistics ---
Decimal integers     : 1
Octal integers       : 0
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 0
Lexical errors       : 1
Sum of integers      : 9
Largest integer      : 9
REJECT"

# ------------------------------------------------------------------------------
# Test 11: Empty Input
# ------------------------------------------------------------------------------
run_test "Empty Input" \
"" \
"--- Numeric Literal Statistics ---
Decimal integers     : 0
Octal integers       : 0
Hexadecimal integers : 0
Binary integers      : 0
Floating-point       : 0
Lexical errors       : 0
Sum of integers      : 0
Largest integer      : 0
ACCEPT"

# ------------------------------------------------------------------------------
# Test 12: Practice Problem 1 Trace
# ------------------------------------------------------------------------------
run_test "Practice Problem 1 Trace" \
"n = 0x1F + 017 + 0b101; // 99
r = 3.5e2 * 09 + 1e;
" \
"1: 0x1F -> HEX_INT (decimal value 31)
1: 017 -> OCTAL_INT (decimal value 15)
1: 0b101 -> BINARY_INT (decimal value 5)
2: 3.5e2 -> FLOAT (value 350)
Lexical Error at line 2: malformed numeric literal '09'
Lexical Error at line 2: malformed numeric literal '1e'
--- Numeric Literal Statistics ---
Decimal integers     : 0
Octal integers       : 1
Hexadecimal integers : 1
Binary integers      : 1
Floating-point       : 1
Lexical errors       : 2
Sum of integers      : 51
Largest integer      : 31
REJECT"

# Cleanup temporary files
rm -f /tmp/test_diff.txt

echo ""
echo "=================================================="
if [ "$passed" -eq "$total" ]; then
    echo -e "FINAL RESULT: ${GREEN}$passed / $total test cases passed!${NC}"
else
    echo -e "FINAL RESULT: ${YELLOW}$passed / $total test cases passed.${NC}"
fi
echo "=================================================="