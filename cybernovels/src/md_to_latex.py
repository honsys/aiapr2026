#!/usr/bin/env python3
import sys
import os
import subprocess
import shutil

def main():
    """
    Converts a Markdown file to a LaTeX file using pandoc.
    """
    # 1. Check if pandoc is installed
    if not shutil.which("pandoc"):
        print("Error: pandoc is not installed or not in the system's PATH.", file=sys.stderr)
        sys.exit(1)

    # 2. Validate command-line arguments
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <input.md>", file=sys.stderr)
        sys.exit(1)

    input_file = sys.argv[1]

    # 3. Check if the input file exists
    if not os.path.isfile(input_file):
        print(f"Error: File not found: {input_file}", file=sys.stderr)
        sys.exit(1)
        
    if not input_file.endswith('.md'):
        print(f"Error: Input file must be a Markdown file (.md): {input_file}", file=sys.stderr)
        sys.exit(1)

    output_file = os.path.splitext(input_file)[0] + ".tex"

    # 4. Construct and run the pandoc command
    command = [
        "pandoc",
        input_file,
        "-s",  # Standalone document
        "-o",
        output_file,
        "--variable", "fontsize=14pt",
        "--variable", "documentclass=extarticle",
        "--variable", "geometry:margin=1in",
    ]

    try:
        result = subprocess.run(command, check=True, capture_output=True, text=True)
        print(f"Successfully converted '{input_file}' to '{output_file}'")
    except subprocess.CalledProcessError as e:
        print(f"Error during pandoc conversion:", file=sys.stderr)
        print(e.stderr, file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
