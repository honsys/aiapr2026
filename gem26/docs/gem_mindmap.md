# Gem Language Mindmap

**Version:** 0.1.0 | **Date:** 2026-04-16 | **Copyright:** David B Hon 2025, 2026

```mermaid
mindmap
  root((Gem Language))
    Keywords
      fun
        Define named function
        Last expr is return value
        Supports recursion & closures
      obj
        Define object / class
        Single inheritance
        All inherit from sys
      use
        Include Gem module
        AI-translate foreign code
        .py .jl .r .cpp .go .rb .rs .js
      if
        Conditional execution
        if ... else ... end
        No parens required
      while
        Loop while condition true
        while cond ... end
      end
        Close fun / obj / if / while
      alias
        Create name alias
      his
        View session history
      lib
        List loaded modules
      exit
        Exit interpreter
    Types
      int
        Integer variable
        Must initialize: int x = 0
      double
        Floating point
        double pi = 3.14
      string
        Text value
        string s = "hello"
      bool
        Boolean true/false
        bool ok = true
    Scope Rules
      Local
        No underscore prefix
        int count = 0
      Global
        Underscore prefix
        _config = 1
    Object Hierarchy
      sys
        Root of all objects
        print
        args
        async
        sethandler
        host
        exec
        doc
        help
        exit
        langport
        redirect
        app
        math
          sin cos sqrt
          pi
          Symbolic SymPy/Sage
            useSymPy useSage
            diff integrate
            simplify solve
            sym_latex to_latex
            write_latex read_latex
            parse_latex compile_latex
        ai
          prompt
          prompt_native
          useMistral useOllama useGemini
          setKey setHost setPath
          provider model host
        text
          read sub replace
          write_pdf write_pdf_a read_pdf
          read_markdown write_markdown
          read_yaml write_yaml
          read_html write_html
          read_xml write_xml
          read_fits_header read_hdf_header
        geo
          lookup
          distance
          write_geojson
          history
          plot2d plot3d
        astro
          Constants
            G c AU pc ly
            Msun Rsun Lsun Tsun
            Mearth Rearth H0 sigma_sb
          Unit Conversions
            to_ly to_pc to_au
            deg_to_rad rad_to_deg
          Stellar
            luminosity stefan_boltzmann wien
            abs_magnitude distance_modulus
            spectral_class schwarzschild_radius
            escape_velocity
          Orbital
            orbital_period orbital_velocity
            hill_sphere roche_limit
            synodic_period planet
          Solar
            solar_flux solar_wind_pressure
            sunspot_cycle parker_spiral_angle
            solar_activity
          Cosmology
            hubble_distance redshift_velocity
            lookback_time critical_density
          Coordinates
            equatorial_to_galactic
            angular_separation
          Exoplanet
            transit_depth habitable_zone
            equilibrium_temp
          Pulsar
            pulsar_spindown
        fin
          ticker
          high_yield_bonds
          high_yield_etfs
          high_yield_equities
          bs_price greeks
          date calendar
          is_holiday add_days diff_days
          bsm
            Inherits all fin methods
            price_american
        chart
          plot show server
        data
          read_csv mean std
        rex
          match find findall groups
          sub gsub split count
        algo
          add quicksort sort
          now date_add date_diff
        tcp
          listen accept connect
          send recv close
          nic routes
        thread
          wait is_finished
        www
          wget app redirect map2d
          cdn
            Inherits all www methods
            start stats purge config
        nlp
          Delegates to ai.prompt
        bev
          data fit_line param
        file
          write exists
        zip
          compress decompress
        img
          resize
        cpp
          repl exec
        itr
          range while
        k3s
          docker_run docker_ps
          docker_build docker_stop
          k3s_apply k3s_get
          k3s_pods k3s_nodes k3s_logs
        vm
          init up ssh
          status halt destroy
        mobl
          phone
            dictate
            make_feature
          Routes
            / PWA HTML
            /log POST waypoint
            /data GET GeoJSON
        trek
          new add edit remove
          load show export_gpx stats
        seo
          index analyze
          Signals
            title description keywords
            og_title og_description
            canonical word_count
            img_count img_with_alt
            internal_links external_links
            h1 h2 h3
        drvr
          linux win11 macos android
          build deploy
    Polyglot Interop
      use keyword
        AI-translate on-the-fly
        .py .jl .r .cpp .go .rb .rs .js
      go
        run build
      ruby
        run
      node
        run npm_install
      rust
        run cargo_new
    CLI Options
      gem file.g
        Run script
      gem -c main.g
        Compile to binary
      gem -o name
        Output name
      gem -h
        Print history
      gem -t file
        AI-translate to Gem
      gem REPL
        Interactive mode
    Platforms
      macOS
      Linux
      Windows 11
        MSYS2 MinGW-w64
      Android
        Chrome PWA
      iPhone
        Safari PWA
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
    thread["thread"]
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
    go["go"]
    ruby["ruby"]
    node["node"]
    rust["rust"]

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
    sys --> go
    sys --> ruby
    sys --> node
    sys --> rust
```

---

## Keyword Quick Reference

| Keyword | Purpose | Example |
|---------|---------|---------|
| `fun` | Define function | `fun add(a,b) a+b end` |
| `obj` | Define object/class | `obj Dog(name) : Animal ... end` |
| `use` | Import or translate foreign code | `use "script.py"` |
| `if` | Conditional | `if x > 0 ... else ... end` |
| `while` | Loop | `while i < 10 ... end` |
| `end` | Close block | closes `fun`/`obj`/`if`/`while` |
| `alias` | Name alias | `alias myprint = sys.print` |
| `his` | Session history | `his` |
| `lib` | List modules | `lib` |
| `exit` | Exit interpreter | `exit` |

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
| `sys` | System root | `print`, `async`, `exec`, `app`, `help` |
| `math` | Math + symbolic | `sin`, `diff`, `integrate`, `compile_latex` |
| `ai` | AI/LLM | `prompt`, `useMistral`, `useOllama` |
| `text` | Documents | `read`, `write_pdf`, `read_yaml`, `read_xml` |
| `geo` | GIS | `lookup`, `distance`, `plot2d`, `plot3d` |
| `astro` | Astrophysics | `luminosity`, `orbital_period`, `hubble_distance` |
| `fin` | Finance | `ticker`, `bs_price`, `greeks` |
| `bsm` | Options | `price_american` (extends `fin`) |
| `chart` | Plotting | `plot`, `show`, `server` |
| `data` | Data science | `read_csv`, `mean`, `std` |
| `rex` | Regex | `match`, `findall`, `gsub`, `split` |
| `algo` | Algorithms | `quicksort`, `now`, `date_add` |
| `tcp` | Networking | `listen`, `connect`, `send`, `recv` |
| `thread` | Concurrency | `wait`, `is_finished` |
| `www` | Web server | `app`, `wget`, `redirect` |
| `cdn` | Caching proxy | `start`, `stats`, `purge` (extends `www`) |
| `nlp` | NLP | delegates to `ai.prompt` |
| `bev` | Curve fitting | `data`, `fit_line`, `param` |
| `file` | Filesystem | `write`, `exists` |
| `zip` | Compression | `compress`, `decompress` |
| `img` | Images | `resize` |
| `cpp` | C++ JIT | `exec`, `repl` |
| `itr` | Iterators | `range`, `while` |
| `k3s` | Docker/K8s | `docker_run`, `k3s_pods` |
| `vm` | Vagrant VMs | `init`, `up`, `ssh`, `halt` |
| `mobl` | Mobile PWA | `phone`, `dictate`, `make_feature` |
| `trek` | Travel logs | `new`, `add`, `show`, `export_gpx` |
| `seo` | SEO analysis | `index`, `analyze` |
| `drvr` | Device drivers | `linux`, `win11`, `macos`, `android` |
| `go` | Go polyglot | `run`, `build` |
| `ruby` | Ruby polyglot | `run` |
| `node` | Node.js polyglot | `run`, `npm_install` |
| `rust` | Rust polyglot | `run`, `cargo_new` |
