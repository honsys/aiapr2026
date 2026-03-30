# ── detect C++ compiler ───────────────────────────────────────────────────────
CXX := $(shell \
    for cc in clang++ g++-14 g++-13 g++ c++; do \
        command -v $$cc >/dev/null 2>&1 && echo $$cc && break; \
    done)

ifeq ($(CXX),)
    $(error No C++ compiler found. Install clang++ or g++)
endif

CXXFLAGS := -std=c++26 -O2
LDFLAGS  := -lcurl
TARGET   := geoip_last
SRC      := geoip_last.cpp

PY_SRC   := geoip_last.py
PY_BIN   := geoip_last_py

UNAME := $(shell uname)

# ── platform tweaks ───────────────────────────────────────────────────────────
ifeq ($(UNAME), Darwin)
    BREW_CURL := $(shell command -v brew >/dev/null 2>&1 && brew --prefix curl 2>/dev/null || echo /usr)
    CXXFLAGS  += -I$(BREW_CURL)/include
    LDFLAGS   := -L$(BREW_CURL)/lib $(LDFLAGS)
    PY_EXT    := $(shell python -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))")
else
    PY_EXT    := $(shell python -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))")
endif

PY_SO := geoip_last$(PY_EXT)

# ── targets ───────────────────────────────────────────────────────────────────
all: $(TARGET) $(PY_BIN) cython

# Install all dependencies
dep:
	@echo "==> Installing dependencies..."
ifeq ($(UNAME), Darwin)
	@command -v brew >/dev/null 2>&1 || (echo "Error: Homebrew not found. Install from https://brew.sh" && exit 1)
	@brew list curl >/dev/null 2>&1 || brew install curl
	@echo "✓ curl installed via Homebrew"
else ifeq ($(shell . /etc/os-release 2>/dev/null && echo $$ID), ubuntu)
	@sudo apt-get update && sudo apt-get install -y libcurl4-openssl-dev python3-dev
	@echo "✓ libcurl4-openssl-dev and python3-dev installed"
else ifeq ($(shell . /etc/os-release 2>/dev/null && echo $$ID), fedora)
	@sudo dnf install -y libcurl-devel python3-devel
	@echo "✓ libcurl-devel and python3-devel installed"
else
	@echo "Unsupported Linux distro. Manually install: libcurl-dev, python3-dev"
	@exit 1
endif
	@python -m pip install --upgrade pip setuptools wheel
	@python -m pip install cython nuitka
	@echo "✓ Python packages installed (cython, nuitka)"
	@echo "==> All dependencies installed. Run 'make' to build."

# C++26 binary
$(TARGET): $(SRC)
	@echo "Building C++ binary with $(CXX)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Nuitka standalone binary
$(PY_BIN): $(PY_SRC)
	python -m nuitka --onefile --output-filename=$@ $<

# Cython shared extension + thin runner script
cython: $(PY_SO)

$(PY_SO): $(PY_SRC) setup.py
	python setup.py build_ext --inplace --quiet

# Tiny CLI wrapper that imports the compiled Cython extension
geoip_last_cython: $(PY_SO)
	@printf '#!/usr/bin/env python\nimport geoip_last; geoip_last.main()\n' > $@
	@chmod +x $@

# ── clean ─────────────────────────────────────────────────────────────────────
clean:
	rm -f $(TARGET) $(PY_BIN) geoip_last_cython
	rm -f geoip_last.c geoip_last$(PY_EXT)
	rm -rf build __pycache__ *.egg-info
	rm -rf $(PY_BIN).build $(PY_BIN).dist $(PY_BIN).onefile-build

.PHONY: all dep cython clean
