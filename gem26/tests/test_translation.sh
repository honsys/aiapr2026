#!/bin/bash

# Test runner for Gem -t translation feature
GEM="./gem"
TEST_DIR="tests/translation"
LANGS=("py" "jl" "c" "cpp" "cc" "rb" "rs" "js")

echo "Starting translation tests..."

for lang in "${LANGS[@]}"; do
    input="${TEST_DIR}/test.${lang}"
    output="${TEST_DIR}/test_${lang}.g"
    
    echo "----------------------------------------"
    echo "Translating ${input} to ${output}..."
    $GEM -t $input -o $output
    
    if [ -f "$output" ]; then
        echo "SUCCESS: ${output} created."
        echo "Content preview:"
        head -n 5 "$output"
    else
        echo "FAILURE: ${output} not created."
    fi
done

echo "----------------------------------------"
echo "Translation tests completed."
