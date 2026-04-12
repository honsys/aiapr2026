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
- **Mobile & Cross-Platform**: Browser-based PWA interface works on Android, iPhone, macOS, Linux, and Windows 11.

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
    *   `app(port, [routes])`: Starts a background web app server with optional routes map. Routes may map paths to HTML strings, file paths, or handler functions.
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
    *   `lookup()`: Returns current location object with `.lat`, `.lon`, `.city`, `.country`.
    *   `distance(lat1, lon1, lat2, lon2)`: Haversine distance in km.
    *   `write_geojson(path, features)`: Writes a GeoJSON FeatureCollection.
    *   `history(plate)`: AI-assisted tectonic history.
    *   `plot2d(data, [layout])`, `plot3d(data, [layout])`: 2D/3D Plotly map visualizations.

### Mobile & Cross-Platform (`mobl`)
*   **`mobl`**: Mobile/desktop cell phone object. Use `mobl.gm` module.
    *   `obj mobl(device_name)`: Instantiate with a device label.
    *   `dictate(spoken_text)`: NLP parse of speech → JSON `{title, note, tags}` via `ai.prompt()`.
    *   `make_feature(lat, lon, spoken_text)`: Builds a GeoJSON Feature from browser-supplied GPS + dictation.
    *   **Cross-platform delivery**: Gem runs the server (`sys.app`); any browser (Android Chrome, iPhone Safari, macOS/Linux/Win11) connects as a PWA using the Web Speech API and Geolocation API.
    *   See `mobl.gm`, `travel_log.g`, `travel_log.html` for the reference implementation.

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
    *   `useGemini()`: Switch back to Gemini.
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

Available topics: `sys`, `math`, `ai`, `text`, `algo`, `bev`, `file`, `zip`, `nlp`, `img`, `geo`, `mobl`, `cpp`, `tcp`, `itr`, `data`, `container`, `vm`, `go`, `ruby`, `node`, `rust`, `fin`, `bsm`, `chart`, `fun`, `obj`, `if`, `while`, `alias`, `his`, `lib`, `exit`.

## Getting Started

### Installation
Gem supports macOS (Intel/ARM), Linux, and Windows 11 (WSL or native).
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

## Mobile Travel Log App

Gem ships with a reference cross-platform mobile/desktop travel log app:

```bash
./gem travel_log.g          # start server on :8080
# Android/iPhone → http://<host-ip>:8080
# macOS/Linux/Win11 → http://localhost:8080
```

Features: NLP voice dictation → AI parsing → GPS pinning → live GeoJSON → Plotly map.

## Future Roadmap

### Phase 1: Core Performance & Tooling (Q2-Q3 2026)
*   **Bytecode VM**: Stack-based bytecode VM for 10x performance gains.
*   **`gpm` (Gem Package Manager)**: Native dependency and module management.
*   **LSP Support**: Language Server Protocol for VS Code and other editors.
*   **Native JIT**: Optional LLVM JIT for compute-heavy loops.

### Phase 2: Ecosystem Expansion (Q4 2026 - Q1 2027)
*   **`gem.web`**: Async web framework inspired by FastAPI and Go Fiber.
*   **`gem.df`**: Native C++ DataFrames (Pandas alternative).
*   **`gem.ui`**: Cross-platform GUI framework based on Skia.
*   **Extended Polyglot**: WASM modules, Swift/Kotlin interop.

### Phase 3: Intelligence & Autonomy (Q2 2027+)
*   **`gem.agent`**: Multi-agent AI framework with tool-use.
*   **WASM Target**: Compile Gem to WebAssembly.
*   **GPU Acceleration**: CUDA/Metal support in `math` and `data`.

## Context: Language Evolution and Trends

### A Brief History
From **Fortran (1957)** and **C (1972)** through the OOP era of **C++ (1985)** and the productivity era of **Python (1991)**, Gem represents the next step: the **STEM-Native Language** fusing C++26 performance with AI-driven development.

### Current Trends in 2026
*   **AI Integration**: Languages evolving from "syntax for humans" to "interfaces for agents."
*   **Universal Runtime**: WebAssembly as the universal safe execution target.
*   **Memory Safety**: Smart-pointer-based safety (C++26, Gem) over manual management.
*   **Polyglot Programming**: Multi-language interop is a requirement, not a luxury.
*   **Mobile-First Web**: PWA + browser APIs (Speech, Geolocation) as the universal mobile UI layer.

---
*Developed with Gemini CLI and Kiro.*
