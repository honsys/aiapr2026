# geoip_last

Reads login records from `/var/log/wtmp` (the same source as the Unix `last` command) and prints the GeoIP city and country for each remote IP address.

## Files

| File | Description |
|---|---|
| `geoip_last.cc` | C++23/26 implementation |
| `geoip_last.py` | Pure Python implementation (also a importable module) |
| `setup.py` | Cython build configuration |
| `Makefile` | Cross-platform build, dep, test targets |
| `Vagrantfile` | Ubuntu VM for Linux testing from macOS |

## Usage

```bash
# build everything
make dep   # install system deps + Python venv
make       # build C++ binary, Nuitka binary, Cython extension

# run
sudo ./geoip_last                  # C++ binary
sudo ./geoip_last_py               # Nuitka-compiled Python binary
sudo python geoip_last.py          # plain Python

# point at a specific wtmp file
./geoip_last /var/log/wtmp.1
```

### As a Python module

```python
from geoip_last import iter_logins, geoip_lookup

for ip in iter_logins("/var/log/wtmp"):
    city, country = geoip_lookup(ip)
    print(ip, city, country)
```

## Build targets

| Target | Description |
|---|---|
| `make dep` | Install system libs (curl) + create `.venv` with Cython/Nuitka |
| `make` | Build all: C++ binary, Nuitka binary, Cython `.so` |
| `make geoip_last` | C++ binary only |
| `make geoip_last_py` | Nuitka standalone binary only |
| `make cython` | Cython `.so` extension only |
| `make test` | macOS: smoke tests locally; Linux: runs full build+test in Vagrant VM |
| `make clean` | Remove build artifacts (keeps `.venv`) |
| `make distclean` | Remove everything including `.venv` |

## Platform support

- **macOS**: uses `utmpx.h` (Apple's preferred API), Homebrew curl, clang++ or g++
- **Linux**: uses `utmp.h`, system libcurl, g++ or clang++
- C++ standard auto-detected: prefers `c++26`, falls back to `c++23`, then `c++2b`
- Python venv isolated per-project under `.venv/`
- Linux tests run inside a Vagrant Ubuntu 22.04 VM (`vagrant up` handled automatically by `make test`)

## Dependencies

**System**
- macOS: `brew install curl`
- Ubuntu/Debian: `apt install libcurl4-openssl-dev python3-dev python3-venv`
- Fedora/RHEL: `dnf install libcurl-devel python3-devel`

**Python** (installed automatically into `.venv` by `make dep`)
- `cython`
- `nuitka`
- `setuptools`, `wheel`

**GeoIP API**: [ip-api.com](http://ip-api.com) — free, no API key required, rate-limited to 45 req/min on the free tier.

---

## Roadmap

### Near-term

- [ ] **IPv6 private range filtering** — extend the private-IP skip list to cover full IPv6 ULA/link-local ranges (`fc00::/7`, `fe80::/10`)
- [ ] **Rate limiting** — respect ip-api.com's 45 req/min free tier limit; add a token bucket or simple sleep between lookups
- [ ] **Offline GeoIP database** — add optional support for MaxMind GeoLite2 (`libmaxminddb`) to avoid network calls entirely
- [ ] **JSON / CSV output** — `--format json|csv|table` flag for scripting and pipeline use
- [ ] **`--since` / `--until` flags** — filter wtmp entries by date range, matching `last -s` / `last -t` behaviour

### Medium-term

- [ ] **macOS `utmpx` path** — macOS doesn't write to `/var/log/wtmp` by default; add support for reading from `utmpx` live database via `getutxent(3)` as a fallback
- [ ] **Async lookups** — use `libcurl multi` (C++) / `asyncio` + `aiohttp` (Python) to parallelise GeoIP requests
- [ ] **CI pipeline** — GitHub Actions matrix: macOS + Ubuntu, g++13/14 + clang16/17, Python 3.11/3.12
- [ ] **Homebrew formula** — package the C++ binary for easy macOS install

### Recommendations

- **Replace ip-api.com with MaxMind GeoLite2** for production use — the free API has rate limits and requires network access; the local DB is faster and works offline
- **Sign the Nuitka binary** on macOS with `codesign` to avoid Gatekeeper prompts
- **Add `--help`** to both binaries — currently they silently open wtmp if given an unknown argument
- **Persist the GeoIP cache** to a local SQLite file between runs to avoid re-querying the same IPs
