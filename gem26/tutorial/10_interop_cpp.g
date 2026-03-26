# Gem Tutorial 10: Interop and C++26
# ------------------------------------

# 1. Using the 'use' keyword
# The 'use' keyword translates source code from other languages
# (Python, Julia, R, Fortran, C++) into native Gem language
# via the internal AI engine.

# use my_script.py
# use my_model.r
# use my_simulation.for
# use my_math.jl
# use my_algorithm.cpp

# 2. C++26 REPL and Direct Execution
# Gem includes a 'cpp' object that integrates with Cling.

# cpp.repl() starts an interactive C++26 session directly in Gem
# cpp.repl() 

# 3. Direct C++ Execution
# You can execute C++ code strings using Cling for maximum performance.
cpp_code = "#include <iostream> 
 int main() { std::cout << "Hello from Cling!" << std::endl; return 0; }"
cpp.exec(cpp_code)

# 4. Math Integration
# You can also use C++ for heavy calculations
calc_code = "double x = 10.5; double y = 20.0; std::cout << x + y << std::endl;"
cpp.exec(calc_code)
