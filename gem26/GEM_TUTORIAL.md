# Gem Language Reference & Tutorial

Welcome to the **Gem Language**, a modern, high-performance scientific and automation scripting language implemented in C++26. Gem integrates powerful libraries like ImageMagick, PDF-Writer, QuantLib, Plotly.js, and Docker directly into its core.

---

## 1. Reserved Keywords

| Keyword | Description |
|:---|:---|
| `fun` | Starts a function definition. Functions are first-class. |
| `obj` | Starts an object/class definition or creates a generic object. |
| `end` | Closes a `fun`, `obj`, `if`, or `while` block. |
| `if` | Conditional execution based on a boolean or non-zero value. |
| `while` | Standard loop that continues as long as a condition is true. |
| `use` | Translates and executes foreign code (Python, Julia, C++, etc.) or Gem modules. |
| `lib` | Displays available builtin libraries. |
| `his` | Displays session history and prompts for prior history files. |
| `alias` | Creates shorthand for expressions (e.g., `alias ? = sys.help()`). |
| `int` | Declares an integer variable (requires initial value). |
| `double` | Declares a double variable (requires initial value). |
| `string` | Declares a string variable (requires initial value). |
| `bool` | Declares a boolean variable (requires initial value). |
| `exit` | Exits the interpreter or REPL. |

---

## 2. Core Syntax & Concepts

### Variables and Scope
- **Mandatory Initialization**: All variable declarations must provide an initial value.
- **Local Variables**: Any variable name (e.g., `x`, `data`) is local to the current scope.
- **Global Variables**: Variables prefixed with an underscore (e.g., `_config`) are globally accessible.
- **Multiple Assignment**: Gem supports chained assignments and typed multiple declarations.
  ```gem
  int x, y, z = 100
  a = b = c = 0
  ```

### Objects and Inheritance
- **Auto-Inheritance**: All objects automatically inherit from the `sys` builtin object.
- **Single Inheritance**: Use `obj Name(params) : Parent` to inherit from another class.
- **Attribute Access**:
    - `.aval` accesses the object's own attribute (or `this.aval`).
    - `..aval` accesses the parent class attribute.

---

## 3. Builtin Objects & Functions

### `sys` (System Root)
- `print(...)`: Prints arguments to stdout.
- `args()`: Returns a vector of command-line arguments.
- `host()`: Returns OS information.
- `exec(cmd)`: Executes a shell command.
- `help([topic])`: Interactive help system.
- `doc([path])`: Reads documentation comments or file contents.
- `redirect(url, [port])`: HTTP redirection or background redirect server.
- `app(port, [routes])`: Starts a background web app server. Routes map paths to HTML strings, file paths, or handler functions.
- `exit()`: Functional exit.

### `math` (Mathematical & LaTeX)
- `sin(x)`, `cos(x)`, `sqrt(x)`: Standard math functions.
- `useSymPy()`, `useSage()`: Select symbolic provider (default: SymPy).
- `simplify(expr)`, `diff(expr, [var])`: Symbolic simplification and differentiation.
- `integrate(expr, [var])`, `solve(expr, [var])`: Symbolic integration and equation solving.
- `sym_latex(expr)`: Returns LaTeX for symbolic expressions.
- `to_latex(val)`: Converts values/vectors to LaTeX strings.
- `write_latex(path, content, [standalone])`: Writes TeX files.
- `read_latex(path)`: Reads TeX files.
- `parse_latex(str)`: Extracts math environments ($...$) from strings.
- `compile_latex(path)`: Compiles `.tex` to `.pdf`.
- **Property**: `pi` (approx. 3.14159)

### `ai` (Artificial Intelligence)
- `prompt(text)`: Sends a prompt to the configured AI provider (Gemini/Mistral/Ollama).
- `prompt_native(text)`: High-performance native Mistral C++ bridge.
- `useMistral([model])`: Configures Mistral as the AI provider.
- `useOllama(model, [host])`: Use a local model via Ollama.
- `useGemini()`: Switch back to Gemini.
- `setKey(key)`, `setHost(host)`, `setPath(path)`: API configuration.

### `mobl` (Mobile & Cross-Platform)
Defined in `mobl.gm`. Provides NLP dictation and GeoJSON travel log support via browser PWA.
- `obj mobl(device_name)`: Instantiate with a device label.
- `dictate(spoken_text)`: Parses speech via `ai.prompt()` → JSON `{title, note, tags}`.
- `make_feature(lat, lon, spoken_text)`: Builds a GeoJSON Feature from GPS coordinates + dictation.
- **Cross-platform**: Gem runs the server (`sys.app`); any browser (Android Chrome, iPhone Safari, macOS/Linux/Win11) connects as a PWA using the Web Speech API and Geolocation API.
- See `mobl.gm`, `travel_log.g`, `travel_log.html` for the reference implementation.

### `geo` (GIS & Geolocation)
- `lookup()`: Returns current location object with `.lat`, `.lon`, `.city`, `.country`.
- `distance(lat1, lon1, lat2, lon2)`: Haversine distance in km.
- `write_geojson(path, features)`: Writes a GeoJSON FeatureCollection file.
- `history(plate)`: AI-assisted tectonic plate history.
- `plot2d(data, [layout])`, `plot3d(data, [layout])`: 2D/3D Plotly map visualizations.

### `text` (String Manipulation)
- `read(path)`, `read_pdf(path)`: Extracts text from files or PDFs.
- `sub(s, start, [len])`: Returns a substring.
- `replace(s, old, new)`: Replaces occurrences of a string.
- `write_pdf(path, content)`, `write_pdf_a(path, content)`: Creates PDF files.
- `read_markdown(path)`, `write_markdown(path, text)`.
- `read_yaml(path)`, `write_yaml(path, obj)`.
- `read_html(path)`, `write_html(path, text)`.
- `read_xml(path)`, `write_xml(path, text)`.
- `read_fits_header(path)`, `read_hdf_header(path)`.

### `algo` (Algorithmic Utilities)
- `add(...)`: Sums numeric arguments.
- `quicksort(array)`, `sort(array, [start, end])`: Sorting utilities.
- `now()`: Returns current date/time string.
- `date_add(ts, days)`, `date_diff(ts1, ts2)`: Date manipulation.

### `fin` (Finance & QuantLib)
- `ticker(symbol)`: Fetches real-time data. Returns a `Ticker` object.
- `bs_price(type, strike, spot, rate, vol, time)`: European option price.
- `greeks(type, strike, spot, rate, vol, time)`: NPV, Delta, Gamma, Theta, Vega.
- `date(d, m, y)`, `calendar(name)`: Create QuantLib objects.
- `is_holiday(cal, d, m, y)`, `add_days(d, m, y, n)`, `diff_days(...)`.

### `bsm` (Black-Scholes-Merton)
Inherits from `fin`. Provides American option pricing via PDE.
- `price_american(ticker, type, duration)`: Returns American option price.

### `bev` (Bevington Data Reduction)
- `data(x, y)`: Load x/y data vectors.
- `fit_line()`: Perform linear regression.
- `param(idx)`: Return fit parameter (0=intercept, 1=slope).

### `data` (Data Science)
- `read_csv(path)`: CSV processing into objects.
- `mean(vector)`, `std(vector)`: Basic statistics.

### `itr` (Iterators)
- `range(n)`: Returns a numeric vector `[0, 1, ..., n-1]`.
- `while(cond_func, body_func)`: Functional `while` loop.

### `container` (Docker & Kubernetes)
- `docker_run(image, cmd)`, `docker_ps()`, `docker_build(path, tag)`, `docker_stop(id)`.
- `k3s_apply(yaml)`, `k3s_get(resource)`, `k3s_pods()`, `k3s_nodes()`, `k3s_logs(pod)`.

### `vm` (Vagrant)
- `init(box)`, `up()`, `ssh(cmd)`, `status()`, `halt()`, `destroy()`.

### `tcp` (Networking)
- `listen(port)`, `accept(fd)`, `connect(host, port)`: Socket communication.
- `send(fd, msg)`, `recv(fd, [size])`, `close(fd)`.
- `nic()`, `routes()`: Network configuration analysis.

### `cpp`, `go`, `rust`, `node`, `ruby` (Polyglot)
- `repl()`: Starts an interactive session (for `cpp`).
- `run(code)`: Executes inline code or script for the specific language.
- `go.build(file)`, `rust.cargo_new(name)`, `node.npm_install(pkg)`.

### `zip` (Compression)
- `compress(src, archive)`, `decompress(archive, dest)`: Native ZIP support via miniz.

### `chart` (Interactive Plotting)
- `plot(traces, layout)`: Generates an interactive Plotly.js chart.
- `show(path)`: Opens chart in browser.
- `server(path, [port])`: Serves chart via background web server.

### `img` (ImageMagick)
- `resize(path, w, h, dest)`: Resize an image.

---

## 4. Control Flow

### `if` Statement
```gem
int x = 10
if x > 5
  sys.print("x is large")
end
```

### `while` Loop
```gem
int i = 0
while i < 5
  sys.print("Iteration", i)
  i += 1
end
```

---

## 5. User Defined Functions

```gem
fun square(x)
  res = x * x
end res

val = square(5)
sys.print("Square of 5 is", val)
```

---

## 6. Mobile Travel Log App

Gem ships with a reference cross-platform mobile/desktop travel log app using `mobl`:

```bash
./gem travel_log.g          # start server on :8080
# Android/iPhone → http://<host-ip>:8080
# macOS/Linux/Win11 → http://localhost:8080
```

The app uses:
- **Web Speech API** for voice dictation (Android Chrome, iPhone Safari, desktop browsers)
- **Geolocation API** for GPS coordinates
- **`ai.prompt()`** for NLP parsing of spoken notes → `{title, note, tags}`
- **`geo.write_geojson()`** for GeoJSON output
- **Plotly.js** open-street-map for the live interactive map

---

## 7. CLI Options

- `./gem <script.g>`: Runs a Gem script.
- `./gem -c <main.g> [modules...] [-o output]`: Compiles Gem scripts into a self-contained executable.
- `./gem -h`: Displays today's session history and enables browsing past sessions.
- `./gem -t <file> [-o output]`: Translates Python, Julia, R, C++, etc., to Gem using AI.

---

## 8. Testing

```bash
make test
./gem_test
# or
./gem tests/comprehensive/all_tests.g
```

---
*Developed with Gemini CLI and Kiro.*
