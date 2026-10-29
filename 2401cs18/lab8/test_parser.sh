#!/bin/bash

# Compile the parser
gcc shift_reduce.c -o shift_reduce
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

SCORE=0
TOTAL=0

# Helper function to run and evaluate tests
run_test() {
    local desc="$1"
    local input="$2"
    local expected="$3"
    
    TOTAL=$((TOTAL+1))
    
    # Run the program, strip trailing whitespaces and carriage returns for clean comparison
    actual=$(echo "$input" | ./shift_reduce | sed 's/[[:space:]]*$//')
    expected=$(echo "$expected" | sed 's/[[:space:]]*$//')
    
    if [ "$actual" == "$expected" ]; then
        echo "✅ Test $TOTAL PASSED: $desc"
        SCORE=$((SCORE+1))
    else
        echo "❌ Test $TOTAL FAILED: $desc"
        echo "Input: $input"
        echo "--- EXPECTED ---"
        echo "$expected"
        echo "--- ACTUAL ---"
        echo "$actual"
        echo "----------------"
    fi
}

echo "Starting Parser Edge-Case Tests..."
echo "=================================="

# Test 1: Standard Precedence (* evaluated before +)
read -r -d '' exp1 << EOM
SHIFT: id
REDUCE: id -> E
SHIFT: + SHIFT: id
REDUCE: id -> E
SHIFT: * SHIFT: id
REDUCE: id -> E
REDUCE: E*E -> E
REDUCE: E+E -> E
ACCEPT
EOM
run_test "Precedence (* > +)" "id + id * id" "$exp1"

# Test 2: Standard Precedence (* evaluated before + when * comes first)
read -r -d '' exp2 << EOM
SHIFT: id
REDUCE: id -> E
SHIFT: * SHIFT: id
REDUCE: id -> E
REDUCE: E*E -> E
SHIFT: + SHIFT: id
REDUCE: id -> E
REDUCE: E+E -> E
ACCEPT
EOM
run_test "Precedence (* > + left-heavy)" "id * id + id" "$exp2"

# Test 3: Left-Associativity (+ and +)
read -r -d '' exp3 << EOM
SHIFT: id
REDUCE: id -> E
SHIFT: + SHIFT: id
REDUCE: id -> E
REDUCE: E+E -> E
SHIFT: + SHIFT: id
REDUCE: id -> E
REDUCE: E+E -> E
ACCEPT
EOM
run_test "Left-Associativity (+ and +)" "id + id + id" "$exp3"

# Test 4: Left-Associativity (+ and - equal precedence)
read -r -d '' exp4 << EOM
SHIFT: id
REDUCE: id -> E
SHIFT: + SHIFT: id
REDUCE: id -> E
REDUCE: E+E -> E
SHIFT: - SHIFT: id
REDUCE: id -> E
REDUCE: E-E -> E
ACCEPT
EOM
run_test "Left-Associativity (+ and -)" "id + id - id" "$exp4"

# Test 5: Parentheses overriding precedence
read -r -d '' exp5 << EOM
SHIFT: ( SHIFT: id
REDUCE: id -> E
SHIFT: + SHIFT: id
REDUCE: id -> E
REDUCE: E+E -> E
SHIFT: ) REDUCE: (E) -> E
SHIFT: * SHIFT: id
REDUCE: id -> E
REDUCE: E*E -> E
ACCEPT
EOM
run_test "Parentheses priority" "( id + id ) * id" "$exp5"

# Test 6: Incomplete Syntax (Rejection)
read -r -d '' exp6 << EOM
SHIFT: id
REDUCE: id -> E
SHIFT: + REJECT
EOM
run_test "Incomplete syntax (Missing operand)" "id +" "$exp6"

# Test 7: Extraneous characters (Rejection)
read -r -d '' exp7 << EOM
SHIFT: ( SHIFT: ( SHIFT: id
REDUCE: id -> E
SHIFT: ) REDUCE: (E) -> E
REJECT
EOM
run_test "Unbalanced Parentheses" "( ( id )" "$exp7"

echo "=================================="
echo "Final Score: $SCORE / $TOTAL tests passed."