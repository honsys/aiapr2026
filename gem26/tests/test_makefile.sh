#!/bin/bash

# Test script for Makefile

echo "Running Makefile Tests..."

# Test 1: make help
echo "Test 1: make help"
make help | grep -q "Gem Language Build System"
if [ "$?" == "0" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] 'make help' output unexpected"
fi

# Test 2: make clean
echo "Test 2: make clean"
make clean
if [ ! -f "gem" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] 'gem' binary still exists after 'make clean'"
fi

# Test 3: make all
echo "Test 3: make all"
make all
if [ -f "gem" ]; then
    echo "  [PASS]"
else
    echo "  [FAIL] 'gem' binary not found after 'make all'"
fi

echo "Makefile Tests Completed."
