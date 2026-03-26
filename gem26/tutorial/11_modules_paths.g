# Gem Tutorial 11: Module Search Paths
# ------------------------------------

# 1. Automatic Search in ~/.gem/lib/
# The 'use' keyword automatically searches in the ~/.gem/lib/ directory
# if a module is not found in the current directory.

# 2. Extensionless Loading
# You can 'use' a module by its name without providing the .gm extension.
# Gem will automatically try appending .gm if no extension is found.

# Example:
# This will search for 'math_ext.gm' in current dir, then in ~/.gem/lib/
use math_ext

sys.print("Loaded math_ext from ~/.gem/lib/")

# Now we can use functions from that module
add_ten(5) # Should print 15
add_ten(100) # Should print 110

# 3. Benefits
# This allows you to build a personal library of Gem modules that are
# accessible from any script or REPL session on your system.
