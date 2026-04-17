# Gem Tutorial 21: Polyglot Interop & Translation
# ---------------------------------------------
# Gem can integrate code from Python, Cython, Julia, R, Fortran, C++,
# Go, Ruby, Node.js, and Rust — either via AI translation ('use') or
# direct native execution (python.run, cython.build, go.run, etc.)

# ---------------------------------------------------------------
# 1. Python: translate-and-run (AI bridge)
# ---------------------------------------------------------------
# 'use' translates Python → Gem on-the-fly using the AI backend.
# imagine 'greet.py' contains:
#   def greet(name):
#       print(f"Hello, {name}!")
#
# use "greet.py"   # → Gem fun greet(name) ... end

# ---------------------------------------------------------------
# 2. Python: native execution (no translation)
# ---------------------------------------------------------------
sys.print("=== Python native run ===")
python.run("print('Hello from Python 3!')")
python.run("import sys; print('Python', sys.version)")

# ---------------------------------------------------------------
# 3. Python: compile .py → .pyc
# ---------------------------------------------------------------
# Creates optimised bytecode, avoids repeated parsing overhead.
# python.compile("mymodule.py")   # → __pycache__/mymodule.pyc
sys.print("Compiling Python module to bytecode...")
# python.compile("mymodule.py")   # uncomment with a real .py file

# ---------------------------------------------------------------
# 4. Python: pip package management
# ---------------------------------------------------------------
# python.pip("numpy")    # install numpy
# python.pip("cython")   # install cython

# ---------------------------------------------------------------
# 5. Cython: compile .pyx → C → .so shared library
# ---------------------------------------------------------------
# Cython is a superset of Python that transpiles to C for speed.
# Pipeline:  .pyx → cython -3 → .c → gcc -shared -fPIC → .so
sys.print("=== Cython build pipeline ===")
# cython.compile("fast_math.pyx")          # step 1: .pyx → .c
# cython.build("fast_math.pyx")            # full:   .pyx → .so
# cython.run("fast_math.pyx")              # compile + execute

# Inline Cython snippet
# cython.run("cdef int add(int a, int b):\n    return a + b")

# ---------------------------------------------------------------
# 6. Cython: translate-and-run via 'use' keyword
# ---------------------------------------------------------------
# use "fast_math.pyx"   # AI-translate Cython → Gem, then run

# ---------------------------------------------------------------
# 7. Julia comparison (Math focus)
# ---------------------------------------------------------------
# Imagine 'calc.jl' contains:
# f(x) = x^2 + sqrt(x)
#
# use "calc.jl" translates it to:
# fun f(x)
#   res = (x * x) + math.sqrt(x)
# end res

# ---------------------------------------------------------------
# 8. R comparison (Stats focus)
# ---------------------------------------------------------------
# Imagine 'stats.r' contains:
# model <- lm(y ~ x, data = my_df)
#
# use "stats.r" maps it to Gem's Bevington module:
# bev.data(x, y)
# bev.fit_line()

# ---------------------------------------------------------------
# 9. Fortran legacy code
# ---------------------------------------------------------------
sys.print("Translating legacy Fortran fitting routine...")
# use "fbevington/Chapt06/FitLine.for"

# ---------------------------------------------------------------
# 10. C++ direct execution via Cling JIT
# ---------------------------------------------------------------
sys.print("=== C++ via Cling JIT ===")
cpp.exec("#include <iostream>
void say_hi() { std::cout << \"Hi from JIT C++!\" << std::endl; }
say_hi();")

# ---------------------------------------------------------------
# 11. Go, Ruby, Node.js, Rust native execution
# ---------------------------------------------------------------
# go.run("fmt.Println(\"Hello from Go\")")
# ruby.run("puts 'Hello from Ruby'")
# node.run("console.log('Hello from Node.js')")
# rust.run("println!(\"Hello from Rust\");")

sys.print("\nPolyglot: Python, Cython, Julia, R, Fortran, C++, Go, Ruby, Node, Rust")
sys.print("Use 'help \"python\"' or 'help \"cython\"' for detailed API reference.")
