# Gem Language: The Complete Consolidated Tutorial

This document is the master guide for the **Gem Language** — a modern, high-performance STEM language implemented in C++26. It consolidates all tutorial modules into a single searchable reference.

---

## 1. Language Basics & Core Syntax

```gem
# --- Variables & Types ---
string name = "Gem Programmer"
double version = 1.0
int x, y, z = 100   # Multiple declaration
a = b = c = 0        # Chained assignment

# Global variables start with underscore
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
end res

# --- Vectors ---
int my_data = [10.5, 20.0, 35.7]
sys.print("First element:", my_data[0])
```

---

## 2. Object-Oriented Programming

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
  fun area()
    res = .width * .height
  end res
end

rect = Rectangle(10, 20)
rect.describe()
sys.print("Area:", rect.area())
# .attr  → this attribute
# ..attr → parent attribute
```

---

## 3. Math & Scientific Computing

```gem
val = math.sqrt(144)
sys.print("Pi:", math.pi)

# Symbolic Math (SymPy/Sage)
expr = "(x + 1)**2"
sys.print("Simplified:", math.simplify(expr))
sys.print("Derivative:", math.diff(expr, "x"))
sys.print("Integral:",   math.integrate("x**2", "x"))
sys.print("Solve x²-1:", math.solve("x**2 - 1", "x"))
sys.print("LaTeX:",      math.sym_latex(expr))

# Bevington Data Reduction
bev.data([1, 2, 3], [2, 4, 6])
bev.fit_line()
sys.print("Slope:", bev.param(1))

# LaTeX Generation
math.write_latex("matrix.tex", math.to_latex([1,0,0,1]), true)
math.compile_latex("matrix.tex")
```

---

## 4. AI & NLP

```gem
# Switch providers
ai.useMistral("mistral-small")   # Mistral API
ai.useOllama("llama3")           # Local via Ollama
ai.useGemini()                   # Back to Gemini

# Prompting
response = ai.prompt("Explain quantum entanglement.")
response = ai.prompt_native("Native Mistral C++ bridge call.")

# NLP: parse free text into structured JSON
string spoken = "Arrived at the Golden Gate Bridge, amazing fog, landmark"
string parsed = ai.prompt("Extract travel note. Return JSON {title, note, tags[]}. Text: " + spoken)
sys.print(parsed)
```

---

## 5. Mobile & Cross-Platform (`mobl`)

```gem
use "mobl.gm"

mobl phone("MyPhone")

# NLP dictation → structured JSON
string parsed = phone.dictate("Hiking at Muir Woods, redwood trees, nature")

# Build GeoJSON feature from browser GPS + dictation
# lat/lon supplied by browser Geolocation API via POST /log
string feat = phone.make_feature(37.89, -122.57, "Hiking at Muir Woods")

geo.write_geojson("travel_log.geojson", [feat])
```

**Full PWA server** (Android, iPhone, macOS, Linux, Win11):
```bash
./gem travel_log.g          # serves on :8080
# Mobile: http://<host-ip>:8080
# Desktop: http://localhost:8080
```

Routes:
- `/` → `travel_log.html` — PWA with mic button + live Plotly map
- `/log` → POST handler — receives `{lat, lon, text}`, returns GeoJSON feature
- `/data` → GET handler — returns full GeoJSON FeatureCollection

---

## 6. Polyglot Interop & AI Translation

```gem
# AI Translation (use keyword)
use "logic.py"    # Translates Python → Gem and executes
use "math.jl"     # Translates Julia → Gem and executes
use "stats.r"     # Translates R → Gem and executes

# Bulk porting
sys.langport("*.py", "pyport.gm")

# C++26 JIT via Cling
cpp.exec("#include <iostream>\nvoid hi() { std::cout << \"Hi from JIT!\"; }")
# cpp.repl()   # interactive C++ session
```

---

## 7. Web & Networking

```gem
# Web App — string, file, or handler routes
fun handle_status(req)
  "{\"status\": \"ok\", \"time\": \"" + algo.now() + "\"}"
end

string routes = {
  "/":       "<h1>Home</h1>",
  "/status": handle_status,
  "/map":    "travel_map.html"
}
sys.app(9000, routes)

# Redirect server
sys.redirect("https://google.com", 9001)

# TCP Sockets
server_fd = tcp.listen(8082)
client = tcp.accept(server_fd)
tcp.send(client.fd, "Hello!")
tcp.close(client.fd)
```

---

## 8. GIS & Geolocation

```gem
# Current location
location = geo.lookup()
sys.print(location.city, location.lat, location.lon)

# Haversine distance
double d = geo.distance(34.05, -118.24, 40.71, -74.00)
sys.print("LA → NYC:", d, "km")

# GeoJSON
geo.write_geojson("map.geojson", [feat1, feat2])

# Interactive maps (Plotly)
trace = obj()
trace.type = "scattermapbox"
trace.lat = [34.05, 40.71, 51.50]
trace.lon = [-118.24, -74.00, -0.12]
trace.mode = "markers+text"
trace.text = ["LA", "NYC", "London"]

layout = obj()
layout.mapbox = obj()
layout.mapbox.style = "open-street-map"

geo.plot2d([trace], layout)
geo.plot3d([trace])

# Tectonic history
# sys.print(geo.history("Pacific"))
```

---

## 9. Data Science & DevOps

```gem
# Data Science
rows = data.read_csv("data.csv")
sys.print("Mean:", data.mean([1.0, 2.0, 3.0]))
sys.print("Std:",  data.std([1.0, 2.0, 3.0]))

# DevOps
container.docker_run("hello-world", "")
container.k3s_nodes()
vm.status()
```

---

## 10. Interactive Charting

```gem
# 2D Scatter
trace1 = obj()
trace1.x = [1, 2, 3, 4, 5]
trace1.y = [10, 15, 13, 17, 22]
trace1.type = "scatter"
trace1.mode = "lines+markers"
trace1.name = "Growth"

layout = obj()
layout.title = "Interactive Chart"

path = chart.plot([trace1], layout)
chart.show(path)
# chart.server(path, 8080)

# 3D Scatter
trace3d = obj()
trace3d.x = [1,2,3]; trace3d.y = [4,5,6]; trace3d.z = [1,4,9]
trace3d.type = "scatter3d"
trace3d.mode = "markers"
chart.plot([trace3d], layout)

# Dual Y-axis
t2 = obj()
t2.x = [1,2,3]; t2.y = [100,150,120]
t2.type = "bar"; t2.yaxis = "y2"
yax2 = obj()
yax2.overlaying = "y"; yax2.side = "right"
layout.yaxis2 = yax2
chart.plot([trace1, t2], layout)
```

---

## 11. Quantitative Finance

```gem
nvda = fin.ticker("NVDA")
sys.print("NVDA price:", nvda.price)

risk = fin.greeks("call", 150, 150, 0.05, 0.2, 0.5)
sys.print("NPV:", risk.npv, "Delta:", risk.delta)

# American option via PDE (bsm inherits fin)
p = bsm.price_american("AAPL", "put", "monthly")
sys.print("American put:", p)

# QuantLib calendar
cal = fin.calendar("NYSE")
sys.print("Is holiday:", fin.is_holiday(cal, 4, 7, 2026))
```

---

## 12. File & System Utilities

```gem
# PDF & Text
text.write_pdf("doc.pdf", "Hello World")
content = text.read("hello.txt")
text.write_markdown("readme.md", "# Hello\nThis is Gem.")
text.write_html("index.html", "<h1>Welcome</h1>")
text.write_yaml("config.yaml", { "port": 8080 })

# Scientific data headers
fits = text.read_fits_header("obs.fits")
hdf  = text.read_hdf_header("data.h5")

# ZIP
zip.compress("src_folder", "backup.zip")
zip.decompress("backup.zip", "dest_folder")

# File I/O
file.write("out.txt", "Hello from Gem")
if file.exists("out.txt")
  sys.print("File exists.")
end
```

---

## 13. REPL & CLI Features

```bash
gem <file.g>              # run script
gem -c main.g mod.gm -o app  # compile to binary
gem -t legacy.py          # AI-translate to Gem
gem -h                    # show session history
```

REPL commands:
- `help "math"` — module docs
- `lib` — list loaded modules
- `his` — session history
- `alias ? = sys.help()` — create shortcuts

---

## 14. Testing

```bash
make test
./gem_test
# or
./gem tests/comprehensive/all_tests.g
```

---
*Developed with Gemini CLI and Kiro.*
