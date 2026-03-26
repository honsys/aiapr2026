# Gem Tutorial 21: Polyglot Interop & Translation
# ---------------------------------------------

# Gem can translate code from other languages on-the-fly 
# using the 'use' keyword and an internal AI bridge.

# 1. Python Comparison
# Imagine 'logic.py' contains:
# def greet(name):
#     print(f"Hello, {name}!")
#
# use "logic.py" translates it to:
# fun greet(name)
#   sys.print("Hello,", name)
# end

# 2. Julia Comparison (Math focus)
# Imagine 'calc.jl' contains:
# f(x) = x^2 + sqrt(x)
#
# use "calc.jl" translates it to:
# fun f(x)
#   res = (x * x) + math.sqrt(x)
# end res

# 3. R Comparison (Stats focus)
# Imagine 'stats.r' contains:
# model <- lm(y ~ x, data = my_df)
#
# use "stats.r" maps it to Gem's Bevington module:
# bev.data(x, y)
# bev.fit_line()

# 4. Fortran Comparison (Legacy focus)
# Gem can use the classic Bevington Fortran code directly!
sys.print("Translating legacy Fortran fitting routine...")
# use "fbevington/Chapt06/FitLine.for"

# 5. C++ Multiple Inheritance Translation
# --------------------------------------
# C++: class C : public A, public B { ... };
#
# Gem translates this into a single inheritance chain:
# obj A_Ext : sys (base)
# ...
# end
#
# obj B_Ext : A_Ext
# ...
# end
#
# obj C : B_Ext
# ...
# end
#
# This ensures that 'C' has access to methods and properties 
# from both 'A' and 'B' in Gem's single-inheritance model.

# 6. C++ Direct Execution
# For C++, Gem can either translate it or execute it directly via Cling.
sys.print("Executing C++ directly via Cling...")
cpp.exec("#include <iostream>
void say_hi() { std::cout << \"Hi from JIT C++!\" << std::endl; }
say_hi();")

sys.print("\nPolyglot interop allows Gem to bridge decades of scientific code.")
