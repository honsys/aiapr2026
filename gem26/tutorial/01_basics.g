# Gem Tutorial 01: Basics
# ------------------------

# 1. Variables and Assignment
# All variable declarations MUST provide an initial value.
# Variable types are: string, int, double, bool.
string name = "Gem Programmer"
double version = 1.0
bool is_ready = true

# Multiple declaration with type
# This declares x, y, z as integers and sets them to 100.
int x, y, z = 100

# Chained assignment
# Sets a, b, and c to 0.
a = b = c = 0

# 2. Printing
# Use the sys object (all objects inherit from sys)
sys.print("Hello,", name)
sys.print("Welcome to Gem version", version)
sys.print("Multiple variables:", x, y, z)

# 3. Global Scope
# Variables starting with an underscore are global.
# Local variables are only accessible in their current block/function.
_global_config = "Production"

fun show_scope()
  # Local variable
  string local_var = "Internal"
  sys.print("Inside function, _global_config is:", _global_config)
  sys.print("Inside function, local_var is:", local_var)
end

# 4. Control Flow
# Standard if/else and while loops are supported.
if x > 50
  sys.print("x is large")
end

int count = 0
while count < 3
  sys.print("Count is", count)
  count += 1
end

# 5. Function Return Values
# The 'end' statement can return a value from a function.
fun add_numbers(a, b)
  res = a + b
end res

sum = add_numbers(5, 10)
sys.print("Sum of 5 and 10 is:", sum)

# 6. Vectors
# Numeric arrays (vectors) are supported with [ ] syntax.
# Gem vectors are dynamic and can be accessed via index.
int my_data = [10.5, 20.0, 35.7]
sys.print("Data vector:", my_data)
sys.print("First element:", my_data[0])

# 7. Builtin Tools
# lib: Lists loaded modules
# his: Shows session history
# alias: Creates shorthand for expressions
alias ? = sys.help()

# 8. Builtin Help
# Gem has a comprehensive help system built directly into the language.
# You can use the 'help' keyword or sys.help("topic") function.

# help         # General help and module list
# help "math"  # Detailed help for the math module
# help "fun"   # Documentation for the 'fun' keyword
# help "obj"   # Documentation for object definition

# Functional call
sys.help("sys")
sys.help("ai")

# 9. Exiting
# exit or sys.exit() can be used to terminate the program.
# exit
