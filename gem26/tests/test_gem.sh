#!/bin/bash

# Test script for gem binary

GEM_BIN="./gem"

if [ ! -f "$GEM_BIN" ]; then
    echo "Error: gem binary not found at $GEM_BIN"
    exit 1
fi

echo "Running Gem Binary Tests..."

# Test 1: Basic Print
echo "Test 1: Basic Print"
echo "sys.print(\"hello\")" | $GEM_BIN | grep -q "hello"
if [ "$?" == "0" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] 'hello' not found in output"
fi

# Test 2: Math
echo "Test 2: Math sin(0)"
echo "sys.print(math.sin(0))" | $GEM_BIN | grep -q "0.000000"
if [ "$?" == "0" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] Expected 0.000000"
fi

# Test 3: Variables
echo "Test 3: Variable Assignment"
echo -e "x = 10\nsys.print(x)" | $GEM_BIN | grep -q "10.000000"
if [ "$?" == "0" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] Expected 10.000000"
fi

# Test 4: Alias
echo "Test 4: Alias expansion"
echo -e "alias p = sys.print()\np(\"aliased\")" | $GEM_BIN | grep -q "aliased"
if [ "$?" == "0" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] Expected 'aliased' in output"
fi

# Test 5: Chart Generation
echo "Test 5: Chart Generation"
$GEM_BIN tests/test_chart.g | grep -q "All chart tests passed"
if [ "$?" == "0" ]; then
    echo "  [PASS]"
    rm gem_chart_*.html
else
    echo "  [FAIL] Chart test script failed"
fi

# Test 6: QuantLib Finance
echo "Test 6: QuantLib Finance"
$GEM_BIN tests/test_quantlib.g | grep -q "Success: Option price seems reasonable."
if [ "$?" == "0" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] QuantLib test script failed"
fi

echo "Gem Binary Tests Completed."
