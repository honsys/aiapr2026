# Gem Language Tutorial Series

Welcome to the official tutorial repository for the **Gem Language** — a modern, expressive STEM language built on C++26 for scientific computing, AI, finance, and cross-platform application development.

## Key Features

- **Explicit Typing with Initial Values**: All variable declarations require an initial value (`int x = 0`).
- **Global vs. Local Scope**: Variables starting with `_` (e.g., `_config`) are global; all others are local.
- **Universal Inheritance**: Every object inherits from `sys`, giving immediate access to `sys.print`, etc.
- **First-Class AI Support**: Built-in `ai` object supports Gemini, Mistral, and Ollama.
- **Polyglot Interop**: `use` keyword bridges Python, Julia, R, Fortran, C++ on-the-fly.
- **Mobile & Cross-Platform**: `mobl` object + browser PWA (Web Speech + Geolocation) works on Android, iPhone, macOS, Linux, and Windows 11.
- **Domain-Specific Built-ins**: `fin`, `bsm`, `bev`, `geo`, `data`, `chart`, `mobl` built into the runtime.

---

## Tutorial Roadmap

### 1. The Foundations
- **[01_basics.g](01_basics.g)**: Variables, types, and basic I/O using the `sys` object.
- **[02_math_latex.g](02_math_latex.g)**: Arithmetic operations and LaTeX-style mathematical expressions.
- **[03_text_pdf.g](03_text_pdf.g)**: Text manipulation and PDF document generation.
- **[04_images.g](04_images.g)**: Image processing with ImageMagick.
- **[05_geo_gis.g](05_geo_gis.g)**: GIS, geolocation, GeoJSON, and interactive maps.

### 2. Application Development
- **[06_web_apps.g](06_web_apps.g)**: Building web servers with `sys.app()` and route handlers.
- **[07_ai_nlp.g](07_ai_nlp.g)**: NLP and AI prompting with Gemini, Mistral, and Ollama.
- **[08_functions.gm](08_functions.gm)**: Defining functions, objects, and inheritance.
- **[09_module_demo.g](09_module_demo.g)**: Structuring code with modules.
- **[11_modules_paths.g](11_modules_paths.g)**: Managing module resolution and import paths.

### 3. Advanced Integration & AI
- **[10_interop_cpp.g](10_interop_cpp.g)**: High-performance C++ interoperability via Cling JIT.
- **[12_lib_management.g](12_lib_management.g)**: Dependency management and library linking.
- **[13_dependency_details.g](13_dependency_details.g)**: Deep dive into Gem's package ecosystem.
- **[14_ai_providers.g](14_ai_providers.g)**: Using Gemini, Mistral, and Ollama side-by-side.
- **[15_mistral_native.g](15_mistral_native.g)**: Native Mistral C++ bridge via `prompt_native`.

### 4. Specialized Tooling
- **[16_algo_text_extras.g](16_algo_text_extras.g)**: Advanced string algorithms and text analysis.
- **[17_repl_features.g](17_repl_features.g)**: Mastering the interactive Gem REPL.
- **[18_tcp_sockets.g](18_tcp_sockets.g)**: Low-level networking fundamentals.
- **[19_tcp_server.g](19_tcp_server.g)** & **[20_tcp_client.g](20_tcp_client.g)**: Client-server architectures.
- **[21_polyglot.g](21_polyglot.g)**: Bridging Python, Julia, and Gem.

### 5. Science, Finance & DevOps
- **[22_builtins_itr_tcp.g](22_builtins_itr_tcp.g)**: Iterators and network protocols.
- **[23_data_science.g](23_data_science.g)**: DataFrames, statistics, and visualization.
- **[24_devops.g](24_devops.g)**: Automation scripts and infrastructure management.
- **[25_ollama_mistral.g](25_ollama_mistral.g)**: Running local LLMs via Ollama.
- **[26_devops_containers_vms.g](26_devops_containers_vms.g)**: Docker and Vagrant environments.

### 6. High-Performance & Symbolic Math
- **[27_rust_node.g](27_rust_node.g)**: Polyglot support for Rust and Node.js.
- **[28_interactive_charts.g](28_interactive_charts.g)**: Dynamic 2D/3D/multi-axes Plotly visualization.
- **[29_quantlib_finance.g](29_quantlib_finance.g)**: Quantitative finance with QuantLib.
- **[30_bsm_pde_inheritance.g](30_bsm_pde_inheritance.g)**: Black-Scholes-Merton PDE solvers.
- **[31_history_and_compilation.g](31_history_and_compilation.g)**: Gem's compilation model and history.
- **[32_symbolic_math.g](32_symbolic_math.g)**: Symbolic differentiation and integration.
- **[33_langport_porting.g](33_langport_porting.g)**: Porting legacy codebases to Gem.
- **[34_geo_tectonics.g](34_geo_tectonics.g)**: Advanced GIS, tectonic modeling, and geo-visualization.
- **[35_text_advanced.g](35_text_advanced.g)**: Large-scale text processing and indexing.

### 7. Mobile & Cross-Platform
- **[36_mobl_travel_log.g](36_mobl_travel_log.g)**: `mobl` object — NLP dictation + GPS + GeoJSON travel log PWA.

### 8. System Programming & Hardware
- **[37_device_drivers.g](37_device_drivers.g)**: `drvr` builtin — Linux, Windows 11, macOS, and Android device driver development.

---

## Help & Builtins

```text
--- Gem Language Help ---
Gem is a high-performance STEM language implemented in C++26.

Usage Tips:
  - All variables must be initialized: 'int x = 0'
  - Global variables start with underscore: '_config = 1'
  - Use 'use "file.py"' to translate and run foreign code.
  - Use 'lib' to see all loaded modules.
  - Use 'his' to view today's session history.

CLI Options:
  gem <file.g>              - Run a script.
  gem -c <main.g> [mod...]  - Compile multiple files into a binary.
  gem -o <name>             - Specify output name for compiler.
  gem -h                    - Print history and exit.
  gem -t <file> [-o output] - AI-assisted translation to Gem.

Available Builtin Modules:
  sys, math, ai, text, algo, bev, file, zip, img, geo, mobl,
  cpp, tcp, itr, data, container, vm, go, ruby, node, rust,
  fin, bsm, chart, astro, drvr

Keywords for Documentation:
  fun, obj, use, alias, his, lib, end, if, while,
  int, double, string, bool, exit

Detailed help: help "topic"  or  help(topic)
```

---

## Mobile Travel Log App

```bash
./gem travel_log.g          # start server on :8080
# Android/iPhone → http://<host-ip>:8080
# macOS/Linux/Win11 → http://localhost:8080
```

Uses `mobl.gm` + `travel_log.html` (PWA). Voice dictation → AI NLP → GPS pin → live GeoJSON → Plotly map.

---

## Getting Started

### Prerequisites

- **macOS/Linux**: `g++` (supporting C++2b/C++26), `make`, `python3`, `libreadline-dev`.
- **Windows 11**: [MSYS2](https://www.msys2.org/) with `mingw-w64-x86_64-gcc`, `make`, and `mingw-w64-x86_64-python`. Ensure `C:\msys64\mingw64\bin` is in your PATH.

### Build and Run

```bash
make all
./gem tutorial/01_basics.g
```

On Windows, the binaries will be produced as `gem.exe` and `gem_test.exe`.

Or enter the REPL:
```bash
./gem
```

---
*Developed with Gemini CLI and Kiro.*
