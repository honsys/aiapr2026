import argparse
import sys
from doconvrt_native import converter

def main():
    parser = argparse.ArgumentParser(description="Native document converter (Python version).")
    parser.add_argument("input", help="Input file.")
    parser.add_argument("-o", "--output", required=True, help="Output file.")
    
    args = parser.parse_args()

    try:
        converter.convert_file(args.input, args.output)
        print(f"Successfully converted '{args.input}' to '{args.output}'")
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
