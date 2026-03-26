#!/bin/bash

set -e

if ! command -v pandoc &> /dev/null; then
    echo "Error: pandoc is not installed. Please install it." >&2
    exit 1
fi

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input.md>" >&2
    exit 1
fi

INPUT_FILE="$1"
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File not found: $INPUT_FILE" >&2
    exit 1
fi

OUTPUT_FILE="${INPUT_FILE%.md}.tex"

pandoc "$INPUT_FILE" \
    -s \
    -o "$OUTPUT_FILE" \
    --variable fontsize=14pt \
    --variable documentclass=extarticle \
    --variable geometry:margin=1in

echo "Successfully converted '$INPUT_FILE' to '$OUTPUT_FILE'"
