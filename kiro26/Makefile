CXX      := g++
CXXFLAGS := -std=c++26 -O2
LDFLAGS  := -lcurl
TARGET   := geoip_last
SRC      := geoip_last.cpp

PY_SRC   := geoip_last.py
PY_BIN   := geoip_last_py

UNAME := $(shell uname)

# ── platform tweaks ───────────────────────────────────────────────────────────
ifeq ($(UNAME), Darwin)
    BREW_CURL := $(shell brew --prefix curl)
    CXXFLAGS  += -I$(BREW_CURL)/include
    LDFLAGS   := -L$(BREW_CURL)/lib $(LDFLAGS)
    PY_EXT    := $(shell python -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))")
else
    PY_EXT    := $(shell python -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))")
endif

PY_SO := geoip_last$(PY_EXT)

# ── targets ───────────────────────────────────────────────────────────────────
all: $(TARGET) $(PY_BIN) cython

# C++26 binary
$(TARGET): $(SRC)
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

.PHONY: all cython clean
