# Gem Tutorial 02: Math, LaTeX & Symbolic Math
# ---------------------------------------------

# 1. Basic Math
val = math.sqrt(144)
sys.print("Sqrt(144) =", val)
sys.print("Pi is approximately", math.pi)

# 2. Symbolic Math (SymPy / SageMath)
# Gem supports symbolic manipulation via internal Python/Sage bridges.
# Default provider is SymPy.
sys.print("\nSymbolic Math:")
expr = "(x + y)**2"
sys.print("Original:", expr)
sys.print("Simplified:", math.simplify(expr))
sys.print("Derivative wrt x:", math.diff(expr, "x"))

# You can switch to SageMath if installed:
# math.useSage()
# sys.print("Sage Simplified:", math.simplify(expr))
# math.useSymPy() # Switch back

# 3. LaTeX Generation
# Convert a vector to a LaTeX matrix
vec = [1, 0, 0, 1]
latex_str = math.to_latex(vec)
sys.print("\nLaTeX Matrix:", latex_str)

# Generate LaTeX for symbolic expression
sym_tex = math.sym_latex("integrate(x**2, x)")
sys.print("Symbolic LaTeX:", sym_tex)

# 4. Writing and Compiling LaTeX
# Create a standalone .tex file
formula = "The identity matrix: " + latex_str + " and integral: $" + sym_tex + "$"
math.write_latex("math_demo.tex", formula, true)

# Compile to PDF (requires pdflatex installed on the system)
# math.compile_latex("math_demo.tex")
sys.print("\nLaTeX file written to math_demo.tex")
