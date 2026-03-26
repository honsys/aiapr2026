# Gem Tutorial 31: History, Compilation, and AI Translation
# This file demonstrates session history, multi-file compilation, and AI-assisted interop.

# 1. Session History
# In the REPL, you can use the 'his' keyword to view today's commands
# and interactively browse prior sessions.
# From the command line, you can use 'gem -h'.

sys.print("Tutorial 31: History, Compilation, and AI Translation")

# Let's perform some operations that will be recorded in history
int x = 100
int y = 200
double z = math.sqrt(x + y)
sys.print("Square root of", x, "+", y, "is", z)

# 2. Multi-file Compilation
# Gem supports bundling a main script with multiple modules into a single binary.
# Command: gem -c main.g module1.gm module2.gm -o my_app

sys.print("\nTo compile this tutorial with a module:")
sys.print("  ./gem -c tutorial/31_history_and_compilation.g tests/test_bar.gm -o tutorial_app")
sys.print("  ./tutorial_app")

# 3. AI-Assisted Translation
# Gem can translate foreign source code (Python, Julia, R, C++, etc.) to Gem on-the-fly.
# This uses the 'ai' module and the '-t' CLI option.

sys.print("\nTo translate a Python script to Gem:")
sys.print("  ./gem -t my_script.py -o my_script.g")
sys.print("  ./gem my_script.g")

# You can also use the 'use' keyword within a script:
# use "my_script.py"

# 4. View History (Interactive)
# sys.print("\nNow showing current session history...")
# his
