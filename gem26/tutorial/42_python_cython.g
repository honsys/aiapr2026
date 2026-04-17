# Gem Tutorial 42: Python & Cython Polyglot
# -----------------------------------------
# Gem provides two modes for Python/Cython interop:
#   1. AI translation mode  — 'use "file.py"' translates to Gem and runs
#   2. Native execution mode — python.run() / cython.build() run natively

# ================================================================
# PYTHON BUILTIN
# ================================================================
# python.run(file_or_code)  — execute .py file or inline string
# python.compile(file)      — byte-compile .py → .pyc (py_compile)
# python.pip(package)       — install via pip

# --- 1. Run a .py file ---
sys.print("=== Run external .py file ===")
# python.run("myscript.py")

# --- 2. Run inline Python code ---
sys.print("=== Inline Python execution ===")
python.run("
import math
x = 2.0
print(f'sqrt({x}) = {math.sqrt(x):.4f}')
")

# --- 3. Python string operations ---
python.run("
s = 'Gem + Python = Power'
print(s.upper())
print(s.replace('Python', 'Cython'))
")

# --- 4. Python NumPy example ---
# python.pip("numpy")
python.run("
try:
    import numpy as np
    a = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
    print('NumPy mean:', a.mean(), ' std:', a.std())
except ImportError:
    print('numpy not installed — run: python.pip(\"numpy\")')
")

# --- 5. Compile .py → .pyc ---
sys.print("=== Compile Python to bytecode ===")
# Writes optimised bytecode to __pycache__/<name>.cpython-3xx.pyc
# python.compile("myscript.py")
# python.compile("mypackage/__init__.py")

# --- 6. pip install ---
# python.pip("requests")
# python.pip("pandas numpy scipy matplotlib")

# ================================================================
# CYTHON BUILTIN
# ================================================================
# Cython is a Python superset that compiles to C for native speed.
# cython.compile(pyx,[out]) — transpile .pyx → C  (step 1)
# cython.build(pyx)         — full pipeline .pyx → .so (steps 1+2)
# cython.run(pyx_or_code)   — compile + execute
# cython.pip(pkg)           — install cython or deps

# Prerequisites:
#   python3-dev (headers), gcc, cython
#   cython.pip("cython")

# --- 7. Compile .pyx → C source ---
sys.print("=== Cython: .pyx → C ===")
# cython.compile("fast_math.pyx")            # writes fast_math.c
# cython.compile("fast_math.pyx", "out.c")   # specify output name

# --- 8. Full build .pyx → .so ---
sys.print("=== Cython: full build .pyx → .so ===")
# cython.build("fast_math.pyx")
# Creates fast_math.so importable from Python:
#   python.run("import fast_math; print(fast_math.add(3, 4))")

# --- 9. Run inline Cython code ---
sys.print("=== Cython inline run ===")
# cython.run creates a temp .pyx, compiles, executes
# cython.run("
# cdef double square(double x):
#     return x * x
# print(square(7.0))
# ")

# --- 10. Real-world: fast C-typed loop ---
# cython.build("primes.pyx")   # where primes.pyx contains:
# def primes(int n):
#     cdef list result = []
#     cdef int i, j, is_prime
#     for i in range(2, n):
#         is_prime = 1
#         for j in range(2, i):
#             if i % j == 0:
#                 is_prime = 0
#                 break
#         if is_prime:
#             result.append(i)
#     return result

# --- 11. Cython + NumPy typed memoryviews ---
# fast_numpy.pyx:
# import numpy as np
# cimport numpy as np
# def sum_array(np.ndarray[double, ndim=1] arr):
#     cdef double total = 0.0
#     cdef int i
#     for i in range(arr.shape[0]):
#         total += arr[i]
#     return total

# --- 12. AI-translate Cython → Gem ---
# use "fast_math.pyx"    # AI-translates Cython → Gem code and runs

# ================================================================
# POLYGLOT COMPARISON TABLE
# ================================================================
# Technique         | Translates? | Compiles? | Native speed?
# ------------------|-------------|-----------|---------------
# use "file.py"     | yes (AI)    | no        | no  (Gem VM)
# python.run()      | no          | no        | Python VM
# python.compile()  | no          | .pyc      | Python VM
# use "file.pyx"    | yes (AI)    | no        | no  (Gem VM)
# cython.compile()  | no          | .c        | C source
# cython.build()    | no          | .so       | native C speed
# cython.run()      | no          | .so+exec  | native C speed
# cpp.exec()        | no          | JIT       | native C++

sys.print("Tutorial 42 complete. See 'help \"python\"' and 'help \"cython\"'.")
