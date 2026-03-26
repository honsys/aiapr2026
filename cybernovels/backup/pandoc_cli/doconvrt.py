#!/usr/bin/env python3
import argparse
import subprocess
import sys
import shutil

def main():
    """
    A command-line tool to convert documents using pandoc.
    """
    parser = argparse.ArgumentParser(
        description="A universal document converter using pandoc.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog="""
Supported formats include:
- markdown (.md)
- latex (.tex)
- pdf (.pdf)
- html (.html)
- plain text (.txt)
        """
    )
    parser.add_argument("input", help="Input file to convert.")
    parser.add_argument("-o", "--output", required=True, help="Output file.")
    
    args = parser.parse_args()

    if not shutil.which("pandoc"):
        print("Error: pandoc is not installed or not in the system's PATH.", file=sys.stderr)
        sys.exit(1)

    pandoc_args = [
        "pandoc", args.input,
        "-o", args.output,
        "--pdf-engine=pdflatex",
        "-V", "fontsize=12pt",
        "-V", "documentclass=article",
        "-V", "geometry:margin=1in",
    ]

    try:
        print(f"Running command: {' '.join(pandoc_args)}")
        result = subprocess.run(pandoc_args, check=True, capture_output=True, text=True)
        print(f"Successfully converted '{args.input}' to '{args.output}'")
        if result.stdout:
            print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error converting file:", file=sys.stderr)
        print(e.stderr, file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
