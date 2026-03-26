# Gem Language: The Complete Consolidated Tutorial

This document serves as the master guide for the **Gem Language**, a modern, high-performance STEM language implemented in C++26. It consolidates all 31 tutorial modules into a single, searchable reference.

---

## 1. Language Basics & Core Syntax
Gem uses a clean, keyword-based syntax. Every variable must be initialized.

```gem
# --- Variables & Types ---
string name = "Gem Programmer"
double version = 1.0
int x, y, z = 100 # Multiple declaration
a = b = c = 0     # Chained assignment

# --- Scope ---
# Global variables start with an underscore
_global_config = "Production"

# --- Control Flow ---
if x > 50
  sys.print("x is large")
end

int count = 0
while count < 3
  sys.print("Count is", count)
  count += 1
end

# --- Functions ---
fun add_numbers(a, b)
  res = a + b
end res # Returns 'res'

# --- Vectors ---
int my_data = [10.5, 20.0, 35.7]
sys.print("First element:", my_data[0])
```

---

## 2. Object-Oriented Programming
Gem supports single inheritance and automatic inheritance from the `sys` object.

```gem
obj Shape(name)
  .name = name
  fun describe()
    sys.print("I am a", .name)
  end
end

obj Rectangle(w, h) : Shape
  .name = "Rectangle"
  .width = w
  .height = h
  
  # Method overriding
  fun describe()
    sys.print("Rectangle:", .width, "x", .height)
  end
  
  fun area()
    res = .width * .height
  end res
end

rect = Rectangle(10, 20)
rect.describe()
sys.print("Area:", rect.area())

# Shorthand: .attr accesses 'this', ..attr accesses parent
```

---

## 3. Math & Scientific Computing
Integrated `math` and `bev` (Bevington) modules.

```gem
# --- Advanced Math ---
val = math.sqrt(144)
sys.print("Pi is", math.pi)

# --- Symbolic Math (SymPy/Sage) ---
# math.useSage() # Optional: switch to SageMath
expr = "(x + 1)**2"
sys.print("Simplified:", math.simplify(expr))
sys.print("Derivative:", math.diff(expr, "x"))
sys.print("Integral:", math.integrate("x**2", "x"))
sys.print("Solution for x**2 - 1 = 0:", math.solve("x**2 - 1", "x"))
sys.print("LaTeX:", math.sym_latex(expr))

# --- Bevington Data Reduction ---
# Load data and fit a line
bev.data([1, 2, 3], [2, 4, 6])
bev.fit_line()
sys.print("Slope:", bev.param(1))

# --- LaTeX Generation ---
vec = [1, 0, 0, 1]
latex_str = math.to_latex(vec)
math.write_latex("matrix.tex", "Identity: " + latex_str, true)
```

---

## 4. AI & Natural Language Processing
Native integration with Gemini, Mistral (API & Cython), and Ollama.

```gem
# --- Switching Providers ---
ai.useMistral("mistral-small")
ai.useOllama("llama3")

# --- Prompting ---
# response = ai.prompt("Explain quantum entanglement.")
# response = ai.prompt_native("Native bridge call.")

# --- NLP ---
text = "Gem is wonderful for science."
cleaned = nlp.clean(text)
sentiment = nlp.sentiment(text)
tokens = nlp.tokenize(text)
```

---

## 5. Polyglot Interop & AI Translation
The `use` keyword and `-t` CLI option enable seamless integration of foreign code.

```gem
# --- AI Translation ---
# use "logic.py"   # Translates Python to Gem and executes
# use "math.jl"    # Translates Julia
# use "stats.r"    # Translates R

# --- Language Porting ---
# sys.langport("*.py", "pyport.gm") # Ports and combines Python files
# sys.langport("script.py")          # Ports script.py to script.g

# --- C++26 JIT (via Cling) ---
cpp.exec("#include <iostream>\nvoid hi() { std::cout << \"Hi from JIT!\"; }")
# cpp.repl() # Starts interactive C++ session
```

---

## 6. Web & Networking
Flask-like web framework and low-level TCP sockets.

```gem
# --- Web App ---
_routes = {
    "/": "<h1>Home</h1>",
    "/api": "{\"status\": \"ok\"}"
}
www.app(9000, _routes)

# --- Redirect Server ---
www.redirect("https://google.com", 9001)

# --- TCP Sockets ---
server_fd = tcp.listen(8082)
client = tcp.accept(server_fd)
tcp.send(client.fd, "Hello!")
tcp.close(client.fd)
```

---

## 7. Data Science & DevOps
CSV handling, statistics, and infrastructure management.

```gem
# --- Data Science ---
rows = data.read_csv("data.csv")
avg_age = data.mean([r.age for r in rows]) # Concept

# --- DevOps ---
container.docker_run("hello-world", "")
container.k3s_nodes()
vm.status()
```

---

## 8. Interactive Charting
2D and 3D visualization using Plotly.js.

```gem
trace = obj()
trace.x = [1, 2, 3]
trace.y = [10, 20, 30]
trace.type = "scatter"

layout = obj()
layout.title = "Growth"

path = chart.plot([trace], layout)
# chart.server(path, 8080) # Serve the chart
```

---

## 9. Quantitative Finance
QuantLib integration for pricing and risk.

```gem
# --- Market Data ---
nvda = fin.ticker("NVDA")

# --- Options & Greeks ---
risk = fin.greeks("call", 150, 150, 0.05, 0.2, 0.5)
sys.print("NPV:", risk.npv, "Delta:", risk.delta)

# --- American Options (PDE) ---
# bsm inherits from fin
p = bsm.price_american("AAPL", "put", "monthly")
```

---

## 10. File & System Utilities
PDF generation, ZIP compression, and GIS.

```gem
# --- PDF & Text ---
text.write_pdf("doc.pdf", "Hello World")
text.write_pdf_a("archival.pdf", "Archival Content")
content = text.read("hello.txt")

# --- Structured Documents ---
text.write_markdown("readme.md", "# Hello\nThis is Gem.")
text.write_html("index.html", "<h1>Welcome</h1>")
text.write_xml("data.xml", "<item>Gem</item>")

# --- Scientific Data Headers ---
fits = text.read_fits_header("obs.fits")
sys.print("BITPIX:", fits.BITPIX)
hdf = text.read_hdf_header("data.h5")

# --- ZIP ---
zip.compress("src_folder", "backup.zip")
zip.decompress("backup.zip", "dest_folder")

# --- GIS & Tectonics ---
geo.write_geojson("map.geojson", [p1, p2])
www.map2d("map.geojson", "map.png")
sys.print("Pacific Plate History:", geo.history("Pacific"))

# --- 2D/3D Geo Visuals ---
trace = obj()
trace.type = "scattergeo"
trace.lat = [34.05, 40.71, 51.50]
trace.lon = [-118.24, -74.00, -0.12]
trace.mode = "markers"
geo.plot2d([trace])
geo.plot3d([trace])
```

---

## 11. REPL & CLI Features
Advanced interactive features and compilation.

- **`help`**: Interactive help system (`help "math"`).
- **`lib`**: Manage personal library in `~/.gem/lib/`.
- **`his`**: Persistent session history.
- **`-c`**: Compile scripts to a standalone binary.
- **`-t`**: Translate files to Gem (`./gem -t logic.py`).

---

## 12. Quality Assurance & Testing
Gem prioritizes reliability through its built-in testing framework.

### The Comprehensive Suite
All language features are verified in `tests/comprehensive/all_tests.g`.

### Building and Running Tests
```bash
# Using the Makefile
make test
./gem_test

# Manual execution via interpreter
./gem tests/comprehensive/all_tests.g
```

### Coverage
The test suite ensures:
1.  **Syntactic Integrity**: Correct parsing of `fun`, `obj`, and control blocks.
2.  **Builtin Robustness**: Functions in `math`, `fin`, `sys`, and `algo` return expected results.
3.  **Cross-Platform Consistency**: Uniform behavior on macOS and Linux.

---
*Developed with Gemini CLI.*
