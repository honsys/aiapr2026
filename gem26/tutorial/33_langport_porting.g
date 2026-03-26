# Gem Tutorial 33: Language Porting with langport()
# This tutorial demonstrates the 'langport' builtin function in the 'sys' module.
# langport() uses AI to translate foreign source code (Python, Julia, C++, etc.) into native Gem.

sys.print("--- Gem langport Builtin Tutorial ---")

# 1. Porting a single Python file
# This will read "script.py", translate it, and save it to "script.g"
# sys.langport("script.py")

# 2. Porting multiple files with a glob pattern
# This will find all .py files in the current directory and port them.
# If no output file is provided, each is saved as [name].g
# sys.langport("*.py")

# 3. Porting and combining into a single Gem module
# This ports all matching files and combines them into "pyport.gm"
# sys.langport("*.py", "pyport.gm")

# 4. Porting other supported languages
# langport support includes .jl (Julia), .r (R), .for (Fortran), and .cpp (C++)
# sys.langport("math_lib.jl", "math_lib.g")

sys.print("\nTutorial 33: langport functionality documented.")
sys.print("Note: langport requires an active AI provider (ai module) configured.")
