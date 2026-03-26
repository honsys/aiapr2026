# Gem Tutorial 17: REPL Features
# ------------------------------

# Gem REPL (Read-Eval-Print Loop) is designed for interactive 
# scientific computing and data exploration.

# 1. The 'help' keyword
# You can use the 'help' keyword directly in the REPL for 
# instant documentation.

# help         # Shows general help and module list
# help math    # Shows functions in the math module
# help "ai"    # Shows AI integration details

# 2. The 'lib' keyword
# Lists all available builtin modules and their internal names.
lib

# 3. Builtin Objects (sys.help)
# The 'help' keyword is a shorthand for 'sys.help()'.
sys.help("bev")

# 4. Global vs Local Scope in REPL
# All variables defined in the REPL are global by default 
# unless inside a function or starting with an underscore.
_config_val = 1.0

# 5. Interactive C++ REPL
# Gem can drop into a native C++26 REPL via Cling:
# cpp.repl()

# 6. Command History
# Gem supports persistent command history with arrow-key navigation.
# Your history is automatically saved to ~/.gem/history.txt
# and reloaded when you start a new session.

sys.print("Tutorial 17: Interactive features and persistent history documented.")
