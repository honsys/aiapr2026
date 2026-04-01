UNAME := $(shell uname)

# ── C++ compiler ──────────────────────────────────────────────────────────────
CXX := $(shell for cc in clang++ g++-14 g++-13 g++; do command -v $$cc >/dev/null 2>&1 && echo $$cc && break; done)
ifeq ($(CXX),)
  $(error No C++ compiler found)
endif

CXX_STD := $(shell for std in c++26 c++23 c++2b; do $(CXX) -std=$$std -x c++ /dev/null -fsyntax-only 2>/dev/null && echo $$std && break; done)
ifeq ($(CXX_STD),)
  $(error Compiler does not support C++23/26)
endif

$(info Using $(CXX) -std=$(CXX_STD))
CXXFLAGS := -std=$(CXX_STD) -O2 -Wall -Wextra -Wpedantic
LDFLAGS  := -lcurl

ifeq ($(UNAME), Darwin)
  BREW_CURL := $(shell brew --prefix curl 2>/dev/null || echo /usr/local)
  CXXFLAGS  += -I$(BREW_CURL)/include
  LDFLAGS   := -L$(BREW_CURL)/lib $(LDFLAGS)
endif

# ── Python venv ───────────────────────────────────────────────────────────────
VENV      := .venv
PYTHON    := $(VENV)/bin/python3
PIP       := $(VENV)/bin/pip
VENV_DONE := $(VENV)/.done

PY_EXT := $(shell test -f $(PYTHON) && $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))" 2>/dev/null || echo .so)

# ── gem lang ──────────────────────────────────────────────────────────────────
GEM       := $(shell command -v gem 2>/dev/null || echo ./gem)
GEM_SRCS  := hontrade.gm hontrade.g
GEM_BIN   := hontrade

# ── targets ───────────────────────────────────────────────────────────────────
all: geoip_last geoip_last_py cython trader gem_build

# ── system + venv deps ────────────────────────────────────────────────────────
deps: $(VENV_DONE)
ifeq ($(UNAME), Darwin)
	@brew list curl >/dev/null 2>&1 || brew install curl
	@echo "✓ macOS deps ready"
else
	@sudo apt-get update -qq && sudo apt-get install -y -q \
	  build-essential libcurl4-openssl-dev python3-dev python3-venv \
	  patchelf ccache
	@echo "✓ Linux deps ready"
endif

$(VENV_DONE):
	python3 -m venv $(VENV)
	$(PIP) install --upgrade pip setuptools wheel
	$(PIP) install cython nuitka publicdotcom-py
	@touch $(VENV_DONE)

# ── C++ binary ────────────────────────────────────────────────────────────────
geoip_last: geoip_last.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# ── Nuitka binary ─────────────────────────────────────────────────────────────
geoip_last_py: geoip_last.py $(VENV_DONE)
	$(PYTHON) -m nuitka --onefile --output-filename=$@ $<

# ── Cython extensions ─────────────────────────────────────────────────────────
cython: geoip_last$(PY_EXT)
trader:  public_trader$(PY_EXT)

geoip_last$(PY_EXT): geoip_last.py setup.py $(VENV_DONE)
	$(PYTHON) setup.py build_ext --inplace --quiet

public_trader$(PY_EXT): public_trader.py setup_trader.py $(VENV_DONE)
	$(PYTHON) setup_trader.py build_ext --inplace --quiet

# ── gem lang build ────────────────────────────────────────────────────────────
gem_build: $(GEM_SRCS)
	@command -v $(GEM) >/dev/null 2>&1 || (echo "gem not found — skipping gem_build" && exit 0)
	$(GEM) -c hontrade.g hontrade.gm -o $(GEM_BIN)
	@echo "✓ gem binary: $(GEM_BIN)"

# ── test ──────────────────────────────────────────────────────────────────────
test: all
ifeq ($(UNAME), Darwin)
	@$(PYTHON) -c "import geoip_last;  print('✓ geoip_last Cython')"
	@$(PYTHON) -c "import public_trader; print('✓ public_trader Cython')"
	@echo "✓ macOS tests passed"
else
	@vagrant up
	@vagrant ssh -c "cd /vagrant && make deps && make all && make test_linux"
endif

test_linux:
	@$(PYTHON) -c "import geoip_last;  print('✓ geoip_last Cython')"
	@$(PYTHON) -c "import public_trader; print('✓ public_trader Cython')"
	@echo "✓ Linux tests passed"

# ── docker ────────────────────────────────────────────────────────────────────
docker_build:
	docker build -t hontrade .

docker_run:
	docker run --rm -p 8099:8099 \
	  -e PUBLIC_API_KEY=$$PUBLIC_API_KEY \
	  -e PUBLIC_ACCOUNT_NUMBER=$$PUBLIC_ACCOUNT_NUMBER \
	  hontrade

# ── clean ─────────────────────────────────────────────────────────────────────
clean:
	rm -f geoip_last geoip_last_py $(GEM_BIN)
	rm -f geoip_last.c geoip_last$(PY_EXT) geoip_last.cpp
	rm -f public_trader.c public_trader$(PY_EXT) public_trader.cpp
	rm -rf build __pycache__ *.egg-info
	rm -rf geoip_last_py.build geoip_last_py.dist geoip_last_py.onefile-build

distclean: clean
	rm -rf $(VENV)

.PHONY: all deps cython trader gem_build test test_linux docker_build docker_run clean distclean
