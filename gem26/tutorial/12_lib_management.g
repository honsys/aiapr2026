# Gem Tutorial 12: Library Management
# -----------------------------------

# 1. The 'lib' keyword
# The 'lib' keyword allows you to manage your personal library in ~/.gem/
# When executed, it:
# - Lists all files and subdirectories in ~/.gem/ (logs, modules, etc.)
# - Prompts you to create a new module from your current session history.

# 2. Creating Modules from History
# If you have been experimenting in the REPL and defined some useful
# functions, you can quickly save them into a reusable module.

# Simply type:
# lib

# Then follow the prompts:
# - Confirm with 'y'
# - Provide a name (e.g., 'utils')

# Gem will extract all code you typed (lines starting with 'gem> ')
# from today's Julian log file and save it to ~/.gem/lib/utils.gm.

# 3. Benefits
# This makes it effortless to transition from experimental REPL code
# to stable, reusable modules that can be 'use'd in any future script.
