# Gem Language (STEM Language)

Gem is a high-performance STEM language designed for scientific computing, data analysis, and modern application development. It is implemented in C++26 and features deep integration with several industry-standard libraries.

## Key Features

- **Performance**: Built on C++26 for maximum speed.
- **Scientific Computing**: Native support for Bevington data reduction, math, and data science.
- **Financial Analysis**: Deep integration with QuantLib and yfinance.
- **Polyglot Interop**: Support for Python, Julia, R, Fortran, C++, Go, Rust, Ruby, and Node.js.
- **Cloud & DevOps**: Native modules for Docker, K3s, Vagrant, and Networking.
- **AI-Powered**: Native integration with Gemini, Mistral, and Ollama (local models).
- **Interactive Visuals**: Interactive 2D/3D multi-axes charting using Plotly.js.

## Built-in Modules and Functions

### System and Core
*   **`sys`**: Root module inherited by all objects.
    *   `print(...)`: Print arguments to stdout.
    *   `args()`: Returns command-line arguments.
    *   `host()`: Returns OS information.
    *   `exec(cmd)`: Executes a shell command.
    *   `help([topic])`: Interactive help system.
    *   `doc([path])`: Reads documentation or script comments.
    *   `langport(pattern, [output])`: Ports foreign code (Python, Julia, C++, etc.) to Gem. Pattern can include `*`.
    *   `redirect(url, [port])`: HTTP redirection and background redirection server.
    *   `app(port, [routes])`: Starts a background web app server with optional routes map.
    *   `exit()`: Exits the interpreter.
*   **`math`**: Constants and functions.
    *   `sin(x)`, `cos(x)`, `sqrt(x)`: Basic math.
    *   `useSymPy()`, `useSage()`: Select symbolic provider.
    *   `simplify(expr)`, `diff(expr, [var])`: Symbolic simplification and differentiation via SymPy/Sage.
    *   `integrate(expr, [var])`, `solve(expr, [var])`: Symbolic integration and equation solving.
    *   `sym_latex(expr)`: Formats symbolic expression for LaTeX.
    *   `to_latex(val)`: Formats values for LaTeX.
    *   `write_latex(path, content, [standalone])`: Writes LaTeX to file.
    *   `read_latex(path)`: Reads content from LaTeX document environment.
    *   `parse_latex(text)`: Extracts math expressions from text ($...$).
    *   `compile_latex(path)`: Generates PDF from LaTeX.
    *   `pi`: The constant π.
*   **`algo`**: Algorithms and time.
    *   `now()`: Current timestamp.
    *   `add(v)`, `sort(v)`, `quicksort(v)`: Vector operations.
    *   `date_add(ts, days)`, `date_diff(ts1, ts2)`: Date arithmetic.
*   **`itr`**: Iterators and Functional Loops.
    *   `range(n)`: Returns a range [0..n-1].
    *   `while(cond_func, body_func)`: Functional loop executing body while condition is true.
*   **`file`**: File system operations.
    *   `write(path, content)`, `exists(path)`.
*   **`zip`**: Native compression (miniz).
    *   `compress(src, arch)`, `decompress(arch, dest)`.
*   **`text`**: String manipulation and document processing.
    *   `read(path)`, `sub(s, start, [len])`, `replace(s, old, new)`.
    *   `write_pdf(path, text)`, `write_pdf_a(path, text)`, `read_pdf(path)`.
    *   `read_markdown(path)`, `write_markdown(path, text)`, `read_yaml(path)`, `write_yaml(path, obj)`.
    *   `read_html(path)`, `write_html(path, text)`, `read_xml(path)`, `write_xml(path, text)`.
    *   `read_fits_header(path)`, `read_hdf_header(path)`.

### Financial and Scientific
*   **`fin`**: Financial engineering (QuantLib & yfinance).
    *   `ticker(symbol)`: Real-time data via yfinance. Returns `Ticker` object.
    *   `bs_price(type, K, S, r, sigma, T)`: European option pricing.
    *   `greeks(type, K, S, r, sigma, T)`: NPV and Delta, Gamma, Theta, Vega.
    *   `date(d, m, y)`, `calendar(name)`: QuantLib date and calendar objects.
    *   `is_holiday(cal, d, m, y)`: Market holiday detection.
    *   `add_days(d, m, y, n)`, `diff_days(d1, m1, y1, d2, m2, y2)`: Financial date arithmetic.
*   **`bsm`**: Black-Scholes-Merton Module.
    *   `price_american(ticker, type, duration)`: PDE-enhanced American pricing.
    *   Inherits all functions from `fin`.
*   **`bev`**: Bevington Data Reduction.
    *   `data(x, y)`, `fit_line()`, `param(idx)`: Linear regression.
*   **`data`**: Data Science and Statistics.
    *   `read_csv(path)`: CSV processing into objects.
    *   `mean(vector)`, `std(vector)`: Basic statistics.
*   **`geo`**: GIS, Geolocation, and GeoJSON.
    *   `lookup()`, `distance(lat1, lon1, lat2, lon2)`, `write_geojson(path, features)`.
    *   `history(plate)`: AI-assisted tectonic history.
    *   `plot2d(data, [layout])`, `plot3d(data, [layout])`: 2D/3D map visualizations.

### Polyglot Interop
*   **`cpp`**: C++ JIT via Cling.
    *   `repl()`: Interactive C++ session.
    *   `exec(code)`: Execute C++ code.
*   **`go`**, **`rust`**, **`node`**, **`ruby`**: Direct interop.
    *   `run(input)`: Execute source file or inline code.
    *   `go.build(file)`, `rust.cargo_new(name)`, `node.npm_install(pkg)`.

### AI and Cloud
*   **`ai`**: AI Integration.
    *   `prompt(text)`, `prompt_native(text)`: Gemini/Mistral prompts.
    *   `useMistral(model)`, `useOllama(model, [host])`: Switch AI providers.
    *   `setKey(key)`, `setHost(host)`, `setPath(path)`: API configuration.
*   **`container`**: Docker and Kubernetes (K3s).
    *   `docker_run(image, cmd)`, `docker_ps()`, `docker_build(path, tag)`, `docker_stop(id)`.
    *   `k3s_apply(yaml)`, `k3s_get(res)`, `k3s_pods()`, `k3s_nodes()`, `k3s_logs(pod)`.
*   **`vm`**: Vagrant Management.
    *   `init(box)`, `up()`, `ssh(cmd)`, `status()`, `halt()`, `destroy()`.
*   **`tcp`**: Networking.
    *   `listen(port)`, `connect(host, port)`, `send(fd, msg)`, `recv(fd, [size])`.
    *   `nic()`, `routes()`: Network interface and routing table analysis.

### Graphics
*   **`chart`**: Interactive Plotly.js charts.
    *   `plot(traces, layout)`, `show(path)`, `server(path, [port])`.
*   **`img`**: ImageMagick integration.
    *   `resize(path, w, h, dest)`.

## Language Keywords and Syntax

Gem uses a clean, keyword-based syntax for object-oriented and functional programming.

- **`fun`**: Define a function.
  ```gem
  fun greet(name)
    sys.print("Hello, " + name)
  end
  ```
- **`obj`**: Define an object/class.
  ```gem
  obj Point(x, y)
    fun magnitude()
      math.sqrt(.x*.x + .y*.y)
    end
  end
  ```
  - **Syntax**: `.aval` accesses own attribute, `..aval` accesses parent attribute.
- **`if` / `while`**: Standard control flow.
- **`use`**: Include files or foreign code.
- **`alias`**: Create REPL shortcuts.
- **`his`**: Display session history.
- **`lib`**: List loaded modules.
- **`exit`**: Terminate session.
- **Type Declarations**: `int`, `double`, `string`, `bool`.

## Help System

The Gem interactive help system is built directly into the language.

- `help`: General help and usage tips.
- `help "topic"`: Detailed help for a specific module or keyword.
- `help("www")`: Functional call to help.

Available topics: `sys`, `math`, `ai`, `text`, `algo`, `bev`, `file`, `zip`, `nlp`, `img`, `geo`, `www`, `cpp`, `tcp`, `itr`, `data`, `container`, `vm`, `go`, `ruby`, `node`, `rust`, `fin`, `bsm`, `chart`, `fun`, `obj`, `if`, `while`, `alias`, `his`, `lib`, `exit`.

## Getting Started

### Installation
Gem supports macOS (Intel/ARM) and Linux.
```bash
make all
```

### Testing
Gem includes a comprehensive test suite verifying all keywords and built-in features.
- **Build & Run Tests**:
  ```bash
  make test
  ./gem_test
  ```
- **Manual Execution**:
  ```bash
  ./gem tests/comprehensive/all_tests.g
  ```

## CLI Options

- `./gem <script.g>`: Run a Gem script.
- `./gem -c <main.g> [mod...] [-o output]`: Compile files into a binary.
- `./gem -h`: Display session history and interactive browser.
- `./gem -t <file> [-o output]`: AI-assisted translation to Gem.

## Future Roadmap

Gem is evolving rapidly to meet the needs of modern STEM research and autonomous systems development.

### Phase 1: Core Performance & Tooling (Q2-Q3 2026)
*   **Bytecode VM**: Transitioning from a token-stream interpreter to a stack-based bytecode virtual machine for 10x performance gains.
*   **`gpm` (Gem Package Manager)**: A native tool to manage dependencies and share community-built modules.
*   **LSP Support**: Language Server Protocol implementation for VS Code and other modern editors.
*   **Native JIT**: Optional JIT compilation using LLVM for compute-heavy loops.

### Phase 2: Ecosystem Expansion (Q4 2026 - Q1 2027)
*   **`gem.web`**: An async, high-performance web framework inspired by FastAPI and Go Fiber.
*   **`gem.df`**: Native C++ DataFrames for high-speed data manipulation (Pandas alternative).
*   **`gem.ui`**: A cross-platform GUI framework based on Skia.
*   **Extended Polyglot**: First-class support for WASM modules and direct Swift/Kotlin interop.

### Phase 3: Intelligence & Autonomy (Q2 2027+)
*   **`gem.agent`**: Built-in framework for creating multi-agent AI systems with tool-use capabilities.
*   **WASM Target**: Compile Gem code directly to WebAssembly for the browser.
*   **GPU Acceleration**: Native support for CUDA/Metal within the `math` and `data` modules.

## Context: Language Evolution and Trends

### A Brief History
The history of programming is a constant push for higher abstraction without sacrificing control. From the raw power of **Fortran (1957)** and **C (1972)** to the object-oriented revolution of **Smalltalk (1980)** and **C++ (1985)**, we reached the productivity era of **Python (1991)** and **JavaScript (1995)**. Gem represents the next logical step: the **STEM-Native Language**, which fuses the performance of C++26 with the flexibility of modern AI-driven development.

### Current Trends in 2026
*   **AI Integration**: Languages are evolving from "syntax for humans" to "interfaces for agents." Gem's native LLM bindings reflect this shift.
*   **Universal Runtime**: WebAssembly is becoming the universal "safe" execution target for all languages.
*   **Memory Safety**: The industry is moving away from manual memory management, favoring either strict ownership models (Rust) or smart-pointer-based safety (C++26, Gem).
*   **Polyglot Programming**: The ability to "use" code from any language (Python, R, Fortran) within a single project is no longer a luxury but a requirement for modern data science.

---
*Developed with Gemini CLI.*
