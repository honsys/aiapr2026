# Gem Tutorial 32: Symbolic Math with SymPy and SageMath
# -----------------------------------------------------
# This tutorial demonstrates Gem's symbolic math capabilities,
# powered by the internal Python bridge, SymPy, and SageMath.

sys.print("Tutorial 32: Symbolic Math")

# 1. Choosing a Provider
# Default is SymPy. You can switch to SageMath if installed.
# math.useSage() 
# math.useSymPy()

# 2. Simplification
# Symbolic expressions are strings that SymPy can understand.
expr = "(x + y)**2 + (x - y)**2"
simple = math.simplify(expr)
sys.print("Original expression:", expr)
sys.print("Simplified:", simple)

# 2. Differentiation
# You can specify the variable to differentiate by.
d_expr = "sin(x) * exp(x)"
derivative = math.diff(d_expr, "x")
sys.print("\nDerivative of", d_expr, "is:")
sys.print(derivative)

# 3. Integration
# Indefinite integration.
i_expr = "x**2 + 3*x + 2"
integral = math.integrate(i_expr, "x")
sys.print("\nIntegral of", i_expr, "is:")
sys.print(integral)

# 4. Equation Solving
# Solves for the specified variable (defaults to x).
# Finds roots of x**2 - 4 = 0
roots = math.solve("x**2 - 4", "x")
sys.print("\nRoots of x**2 - 4 = 0 are:", roots)

# 5. Symbolic LaTeX
# Generates LaTeX code for a symbolic expression.
latex_code = math.sym_latex("(x + 1) / (x - 1)")
sys.print("\nLaTeX for (x+1)/(x-1):", latex_code)

# 6. Combining with File Output
# You can write symbolic results directly to a LaTeX file.
formula = "f(x) = " + math.sym_latex("diff(sin(x)*exp(x), x)")
math.write_latex("derivative.tex", formula, true)
sys.print("\nLaTeX formula written to derivative.tex")
