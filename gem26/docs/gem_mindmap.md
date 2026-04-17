# Gem Language Mindmap

**Version:** 0.1.0 | **Date:** 2026-04-16 | **Copyright:** David B Hon 2025, 2026

```mermaid
mindmap
  root((Gem))
    Keywords
      fun
      obj
      use
      if
      else
      while
      end
      alias
      his
      lib
      exit
      quit
      langport
    Literals
      true
      false
      null
      nil
      nan
    Types
      int
      double
      string
      bool
      _global
    Builtins
      isnil x
      isnan x
      tonum x
      tostr x
    Operators
      Arithmetic + - * /
      Compound += -= *= /=
      Comparison == != > >= < <=
      Logical ! && ||
      Separator ; multi-stmt
      String concat +
    Scope
      Local no underscore
      Global underscore prefix
    System
      sys
        print args async exec
        help helpfull doc host exit
        langport redirect app
      math
        sin cos sqrt pi
        diff integrate simplify
        sym_latex compile_latex
      ai
        prompt prompt_native
        useMistral useOllama useGemini
      text
        read write_pdf read_pdf
        read_yaml write_yaml
        read_html read_xml
    Science
      astro
        Constants G c AU pc
        Stellar luminosity wien
        Orbital period velocity
        Solar flux sunspot
        Cosmology hubble redshift
        Exoplanet transit habitable
      geo
        lookup distance
        write_geojson plot2d plot3d
      math symbolic
        diff integrate solve
        sym_latex compile_latex
      bev
        data fit_line param
      data
        read_csv mean std
    Finance
      fin
        ticker bs_price greeks
        high_yield_bonds etfs equities
        date calendar is_holiday
      bsm extends fin
        price_american
    Web and Network
      www
        app wget redirect map2d
      cdn extends www
        start stats purge config
      tcp
        listen accept connect
        send recv close nic routes
      seo
        index analyze
    DevOps
      k3s
        docker_run docker_ps
        k3s_apply k3s_pods
      vm
        init up ssh halt destroy
      drvr
        linux win11 macos android
    Mobile
      mobl
        phone dictate make_feature
      trek
        new add edit show stats
    Polyglot
      python
        run
        compile py_to_pyc
        pip install
      cython
        compile pyx_to_c
        build pyx_to_so
        run
        pip install
      go run build
      ruby run
      node run npm_install
      rust run cargo_new
      cpp exec repl
    Utilities
      rex
        match find findall
        sub gsub split count
      algo
        quicksort sort now
        date_add date_diff
      itr
        range while
      file write exists
      zip compress decompress
      img resize
      nlp prompt delegate
      thread wait is_finished
      art
        text_to_art art_to_svg
        svg_to_art mindmap readme
    Platforms
      macOS Linux
      Windows 11 MSYS2
      Android Chrome PWA
      iPhone Safari PWA
```

---

## Object Inheritance Diagram

```mermaid
graph TD
    sys["sys (root)"]
    math["math"]
    ai["ai"]
    text["text"]
    geo["geo"]
    astro["astro"]
    fin["fin"]
    bsm["bsm (extends fin)"]
    chart["chart"]
    data["data"]
    rex["rex"]
    algo["algo"]
    tcp["tcp"]
    thread["thread (handle)"]
    www["www"]
    cdn["cdn (extends www)"]
    nlp["nlp"]
    bev["bev"]
    file["file"]
    zip["zip"]
    img["img"]
    cpp["cpp"]
    itr["itr"]
    k3s["k3s"]
    vm["vm"]
    mobl["mobl"]
    trek["trek"]
    seo["seo"]
    drvr["drvr"]
    art["art"]
    go["go"]
    ruby["ruby"]
    node["node"]
    rust["rust"]
    python["python"]
    cython["cython"]

    sys --> math
    sys --> ai
    sys --> text
    sys --> geo
    sys --> astro
    sys --> fin
    fin --> bsm
    sys --> chart
    sys --> data
    sys --> rex
    sys --> algo
    sys --> tcp
    sys --> thread
    sys --> www
    www --> cdn
    sys --> nlp
    sys --> bev
    sys --> file
    sys --> zip
    sys --> img
    sys --> cpp
    sys --> itr
    sys --> k3s
    sys --> vm
    sys --> mobl
    sys --> trek
    sys --> seo
    sys --> drvr
    sys --> art
    sys --> go
    sys --> ruby
    sys --> node
    sys --> rust
    sys --> python
    sys --> cython
```

---

## Keyword Quick Reference

| Keyword | Purpose | Example |
|---------|---------|---------|
| `fun` | Define function | `fun add(a,b) a+b end` |
| `obj` | Define object/class | `obj Dog(name) : Animal ... end` |
| `use` | Import or translate foreign code | `use "script.py"` |
| `if` | Conditional | `if x > 0 ... else ... end` |
| `else` | Alternate branch | `if cond ... else ... end` |
| `while` | Loop | `while i < 10 ... end` |
| `end` | Close block | closes `fun`/`obj`/`if`/`while` |
| `alias` | REPL shortcut | `alias ? = sys.help()` |
| `his` | Session history | `his` |
| `lib` | List modules | `lib` |
| `exit` | Exit interpreter | `exit` |
| `quit` | Exit interpreter (alias) | `quit` |
| `langport` | AI-port foreign code | `langport("*.py", "out.gm")` |
| `true` | Boolean literal | `bool ok = true` |
| `false` | Boolean literal | `bool done = false` |
| `null` | Null/void value | returned by void functions |

## Operator Quick Reference

| Operator | Type | Example |
|----------|------|---------|
| `+` `-` `*` `/` | Arithmetic | `x = a + b * 2` |
| `+=` `-=` `*=` `/=` | Compound assignment | `x += 5` |
| `==` `!=` `>` `>=` `<` `<=` | Comparison | `if x >= 10` |
| `!` | Logical not (prefix) | `while !done` |
| `;` | Statement separator | `int a = 1; int b = 2` |
| `+` (strings) | Concatenation | `"Hello" + " World"` |

## Type Quick Reference

| Type | Declaration | Notes |
|------|-------------|-------|
| `int` | `int x = 0` | Integer; must initialize |
| `double` | `double pi = 3.14` | Float; must initialize |
| `string` | `string s = "hi"` | Text; must initialize |
| `bool` | `bool ok = true` | Boolean; must initialize |
| `_var` | `_config = 1` | Underscore prefix = global scope |

## Builtin Module Summary (34 total)

| Module | Domain | Key Functions |
|--------|--------|---------------|
| `sys` | System root | `print`, `async`, `exec`, `app`, `help`, `langport` |
| `math` | Math + symbolic | `sin`, `diff`, `integrate`, `compile_latex` |
| `ai` | AI/LLM | `prompt`, `useMistral`, `useOllama`, `useGemini` |
| `text` | Documents | `read`, `write_pdf`, `read_yaml`, `read_xml` |
| `geo` | GIS | `lookup`, `distance`, `plot2d`, `plot3d` |
| `astro` | Astrophysics | `luminosity`, `orbital_period`, `hubble_distance`, `pulsar_spindown` |
| `fin` | Finance | `ticker`, `bs_price`, `greeks`, `high_yield_bonds` |
| `bsm` | Options | `price_american` (extends `fin`) |
| `chart` | Plotting | `plot`, `show`, `server` |
| `data` | Data science | `read_csv`, `mean`, `std` |
| `rex` | Regex | `match`, `findall`, `gsub`, `split` |
| `algo` | Algorithms | `quicksort`, `now`, `date_add` |
| `tcp` | Networking | `listen`, `connect`, `send`, `recv`, `nic`, `routes` |
| `thread` | Concurrency | `wait`, `is_finished` |
| `www` | Web server | `app`, `wget`, `redirect`, `map2d` |
| `cdn` | Caching proxy | `start`, `stats`, `purge` (extends `www`) |
| `nlp` | NLP | delegates to `ai.prompt` |
| `bev` | Curve fitting | `data`, `fit_line`, `param` |
| `file` | Filesystem | `write`, `exists` |
| `zip` | Compression | `compress`, `decompress` |
| `img` | Images | `resize` |
| `cpp` | C++ JIT | `exec`, `repl` |
| `itr` | Iterators | `range`, `while` |
| `k3s` | Docker/K8s | `docker_run`, `k3s_pods`, `k3s_apply` |
| `vm` | Vagrant VMs | `init`, `up`, `ssh`, `halt`, `destroy` |
| `mobl` | Mobile PWA | `phone`, `dictate`, `make_feature` |
| `trek` | Travel logs | `new`, `add`, `show`, `export_gpx`, `stats` |
| `seo` | SEO analysis | `index`, `analyze` |
| `drvr` | Device drivers | `linux`, `win11`, `macos`, `android`, `build`, `deploy` |
| `art` | ASCII art/SVG | `text_to_art`, `art_to_svg`, `mindmap`, `readme`, `tutorial` |
| `go` | Go polyglot | `run`, `build` |
| `ruby` | Ruby polyglot | `run` |
| `node` | Node.js polyglot | `run`, `npm_install` |
| `rust` | Rust polyglot | `run`, `cargo_new` |
| `python` | Python 3 polyglot | `run`, `compile`, `pip` |
| `cython` | Cython polyglot | `run`, `compile`, `build`, `pip` |
