# Gem Language Tutorial Series

Welcome to the official tutorial repository for the **Gem Language**. Gem is a modern, expressive, and versatile programming language designed for rapid prototyping, data science, financial modeling, and seamless AI integration.

## Introduction to Gem

Gem is built on the philosophy of "Batteries Included" with a focus on simplicity and power.

### Key Features

-   **Explicit Typing with Initial Values:** Every variable declaration in Gem requires an initial value, ensuring type safety from the start. Supported types include `string`, `int`, `double`, and `bool`.
-   **Global vs. Local Scope:** Gem uses a simple naming convention for scoping: variables starting with an underscore (e.g., `_globalVar`) have global scope, while all others are local.
-   **Universal Inheritance:** Every object in Gem, including user-defined objects, automatically inherits from the `sys` object, providing immediate access to core system functions like `sys.print`.
-   **First-Class AI Support:** Gem features a built-in `ai` object that allows developers to switch between providers (Google Gemini, Mistral AI, etc.) and perform complex prompting tasks directly within the language.
-   **Polyglot Interop:** Use the `use` keyword to bridge and translate logic from other languages like Python and Julia on-the-fly.
-   **Domain-Specific Built-ins:** Specialized modules for Finance (`fin`, `bsm`), Mathematics (`math`), and DevOps are built directly into the runtime.

---

## Tutorial Roadmap

This repository contains a step-by-step guide to mastering Gem, from basic syntax to advanced systems programming.

### 1. The Foundations
-   **[01_basics.g](01_basics.g):** Variables, types, and basic I/O using the `sys` object.
-   **[02_math_latex.g](02_math_latex.g):** Arithmetic operations and LaTeX-style mathematical expressions.
-   **[03_text_pdf.g](03_text_pdf.g):** Text manipulation and PDF document generation.
-   **[04_images.g](04_images.g):** Image processing and basic computer vision tasks.
-   **[05_geo_gis.g](05_geo_gis.g):** Geographic Information Systems (GIS) and coordinate handling.

### 2. Application Development
-   **[06_web_apps.g](06_web_apps.g):** Building simple web servers and reactive applications.
-   **[07_ai_nlp.g](07_ai_nlp.g):** Natural Language Processing using Gem's internal AI engine.
-   **[08_functions.gm](08_functions.gm):** Defining functions, objects, and exploring inheritance.
-   **[09_module_demo.g](09_module_demo.g):** Structuring code with modules.
-   **[11_modules_paths.g](11_modules_paths.g):** Managing module resolution and import paths.

### 3. Advanced Integration & AI
-   **[10_interop_cpp.g](10_interop_cpp.g):** High-performance C++ interoperability.
-   **[12_lib_management.g](12_lib_management.g):** Dependency management and library linking.
-   **[13_dependency_details.g](13_dependency_details.g):** Deep dive into Gem's package ecosystem.
-   **[14_ai_providers.g](14_ai_providers.g):** Using Google Gemini and Mistral AI side-by-side.
-   **[15_mistral_native.g](15_mistral_native.g):** Native optimizations for Mistral models.

### 4. Specialized Tooling
-   **[16_algo_text_extras.g](16_algo_text_extras.g):** Advanced string algorithms and text analysis.
-   **[17_repl_features.g](17_repl_features.g):** Mastering the interactive Gem REPL.
-   **[18_tcp_sockets.g](18_tcp_sockets.g):** Low-level networking fundamentals.
-   **[19_tcp_server.g](19_tcp_server.g) & [20_tcp_client.g](20_tcp_client.g):** Building robust client-server architectures.
-   **[21_polyglot.g](21_polyglot.g):** Bridging Python, Julia, and Gem.

### 5. Science, Finance & DevOps
-   **[22_builtins_itr_tcp.g](22_builtins_itr_tcp.g):** Iterators and network protocols.
-   **[23_data_science.g](23_data_science.g):** Dataframes, statistics, and visualization.
-   **[24_devops.g](24_devops.g):** Automation scripts and infrastructure management.
-   **[25_ollama_mistral.g](25_ollama_mistral.g):** Running local LLMs via Ollama integration.
-   **[26_devops_containers_vms.g](26_devops_containers_vms.g):** Managing Docker and VM environments.

### 6. High-Performance & Symbolic Math
-   **[27_rust_node.g](27_rust_node.g):** Polyglot support for Rust and Node.js.
-   **[28_interactive_charts.g](28_interactive_charts.g):** Dynamic data visualization.
-   **[29_quantlib_finance.g](29_quantlib_finance.g):** Quantitative finance with QuantLib bindings.
-   **[30_bsm_pde_inheritance.g](30_bsm_pde_inheritance.g):** Black-Scholes-Merton models and PDE solvers.
-   **[31_history_and_compilation.g](31_history_and_compilation.g):** Gem's compilation model and execution history.
-   **[32_symbolic_math.g](32_symbolic_math.g):** Symbolic differentiation and integration.
-   **[33_langport_porting.g](33_langport_porting.g):** Porting legacy codebases to Gem.
-   **[34_geo_tectonics.g](34_geo_tectonics.g):** Advanced geological modeling.
-   **[35_text_advanced.g](35_text_advanced.g):** Large-scale text processing and indexing.

---

## Help & Builtins

The Gem runtime includes a powerful built-in help system. You can access it directly from your scripts or the interactive REPL.

### `help` Command Output

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
  gem <file.g>             - Run a script.
  gem -c <main.g> [mod...] - Compile multiple files into a binary.
  gem -o <name>            - Specify output name for compiler.
  gem -h                   - Print history and exit.
  gem -t <file> [-o output]- AI-assisted translation to Gem.

Available Builtin Modules:
  sys, math, ai, text, algo, bev, file, zip, nlp, img, geo, www, cpp, tcp, itr, data, container, vm, go, ruby, node, rust, fin, bsm, chart

Keywords for Documentation:
  fun, obj, use, alias, his, lib, end, if, while, int, double, string, bool, exit

Detailed help: help "topic" or help(topic)
```

### Accessing Module Documentation

To get detailed information about a specific module or keyword, use:
- `help "math"` or `sys.help("math")`
- `help "ai"`
- `help "fun"`

---

## Roadmap & Future Features

Gem is rapidly evolving. Here is our vision for the next major releases:

### Architecture Enhancements
-   **Parallel Execution Engine:** Introducing a multi-threaded actor model for distributed computing.
-   **JIT Compilation:** Transitioning from an interpreted model to a LLVM-based Just-In-Time compiler for near-native performance.
-   **Memory Safety:** Implementing a "borrow checker" inspired system to ensure thread safety without a garbage collector.

### Advanced AI Support
-   **Prompt Editing & Versioning:** Native support for managing prompt templates as code assets, including versioning and A/B testing.
-   **RAG (Retrieval-Augmented Generation) Built-in:** Direct integration with vector databases (like Pinecone/Milvus) via a new `vector` object.
-   **Agentic Framework:** High-level abstractions for building autonomous AI agents that can use Gem's built-in tools (shell, networking, finance).
-   **Fine-tuning Interface:** Ability to trigger and manage model fine-tuning directly from Gem scripts.

### Tooling & Ecosystem
-   **Gem Language Server (GLS):** Enhanced IDE support with real-time AI suggestions and type checking.
-   **Cloud Native Deployment:** One-click deployment to serverless environments (AWS Lambda, Google Cloud Functions).
-   **Visual Debugger:** A web-based debugger for inspecting complex object graphs and AI reasoning chains.

---

## Getting Started

1.  **Install the Gem Runtime:** Follow the instructions at [gem-lang.org](https://gem-lang.org).
2.  **Run a Tutorial:**
    ```bash
    gem 01_basics.g
    ```
3.  **Explore the REPL:** Simply type `gem` to enter interactive mode.

Happy Coding with Gem!
