# Gem Tutorial 09: Modules (.gm) vs Scripts (.g)
# ---------------------------------------------

# In Gem, .gm files are used for libraries/modules.
# While the interpreter currently processes files sequentially,
# the convention is to put reusable code in .gm.

sys.print("This script (.g) uses functions defined in a module (.gm)")

# Assuming 08_functions.gm has been loaded or defined:
# calculate_circle_area(15) 

sys.print("Convention: .g for apps, .gm for libraries.")
