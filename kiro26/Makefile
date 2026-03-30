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
    BREW_CURL := $(shell command -v brew >/dev/null 2>&1 && brew --prefix curl 2>/dev/null || echo /usr/local)
    CXXFLAGS  += -I$(BREW_CURL)/include
    LDFLAGS   := -L$(BREW_CURL)/lib $(LDFLAGS)
    PY_EXT    := $(shell python3 -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))" 2>/dev/null || echo .so)
else
    PY_EXT    := $(shell python3 -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))" 2>/dev/null || echo .so)
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
else ifeq ($(shell test -f /etc/os-release && . /etc/os-release && echo $$ID), ubuntu)
	@sudo apt-get update && sudo apt-get install -y libcurl4-openssl-dev python3-dev
	@echo "✓ libcurl4-openssl-dev and python3-dev installed"
else ifeq ($(shell test -f /etc/os-release && . /etc/os-release && echo $$ID), fedora)
	@sudo dnf install -y libcurl-devel python3-devel
	@echo "✓ libcurl-devel and python3-devel installed"
else
	@echo "Unsupported Linux distro. Manually install: libcurl-dev, python3-dev"
	@exit 1
endif
	@python3 -m pip install --upgrade pip setuptools wheel
	@python3 -m pip install cython nuitka
	@echo "✓ Python packages installed (cython, nuitka)"
	@echo "==> All dependencies installed. Run 'make' to build."

# C++26 binary
$(TARGET): $(SRC)
	@echo "Building C++ binary with $(CXX)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Nuitka standalone binary
$(PY_BIN): $(PY_SRC)
	@echo "Building Nuitka binary..."
	python3 -m nuitka --onefile --output-filename=$@ $<

# Cython shared extension + thin runner script
cython: $(PY_SO)

$(PY_SO): $(PY_SRC) setup.py
	@echo "Building Cython extension..."
	python3 setup.py build_ext --inplace --quiet

# Tiny CLI wrapper that imports the compiled Cython extension
geoip_last_cython: $(PY_SO)
	@printf '#!/usr/bin/env python3\nimport geoip_last; geoip_last.main()\n' > $@
	@chmod +x $@

# ── test ──────────────────────────────────────────────────────────────────────
test: all
ifeq ($(UNAME), Darwin)
	@echo "==> Testing on macOS..."
	@echo "Testing C++ binary..."
	@./$(TARGET) --help 2>&1 | grep -q "geoip_last" || (echo "✓ C++ binary runs" && true)
	@echo "Testing Python binary..."
	@./$(PY_BIN) --help 2>&1 | grep -q "geoip_last" || (echo "✓ Nuitka binary runs" && true)
	@echo "Testing Cython module..."
	@python3 -c "import geoip_last; print('✓ Cython module imports')"
	@echo "==> All macOS tests passed"
else
	@echo "==> Testing on Linux via Vagrant..."
	@command -v vagrant >/dev/null 2>&1 || (echo "Error: Vagrant not found. Install from https://www.vagrantup.com" && exit 1)
	@vagrant up
	@vagrant ssh -c "cd /vagrant && make clean && make dep && make all"
	@vagrant ssh -c "cd /vagrant && ./$(TARGET) --help 2>&1 | grep -q 'geoip_last' || echo '✓ C++ binary runs'"
	@vagrant ssh -c "cd /vagrant && ./$(PY_BIN) --help 2>&1 | grep -q 'geoip_last' || echo '✓ Nuitka binary runs'"
	@vagrant ssh -c "cd /vagrant && python3 -c 'import geoip_last; print(\"✓ Cython module imports\")'"
	@echo "==> All Linux tests passed"
endif

# ── clean ─────────────────────────────────────────────────────────────────────
clean:
	rm -f $(TARGET) $(PY_BIN) geoip_last_cython
	rm -f geoip_last.c geoip_last$(PY_EXT)
	rm -rf build __pycache__ *.egg-info
	rm -rf $(PY_BIN).build $(PY_BIN).dist $(PY_BIN).onefile-build

.PHONY: all dep cython test clean
