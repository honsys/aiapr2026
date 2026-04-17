# Gem Language Makefile
# High-performance STEM language implemented in C++26

CXX = g++
TARGET = gem
RM = rm -rf

# Detect OS and Architecture
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ARCH_FLAGS =
IS_WINDOWS = 0

ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_M),arm64)
        ARCH_FLAGS = -arch arm64
    else
        ARCH_FLAGS = -arch x86_64
    endif
else ifeq ($(UNAME_S),Linux)
    ARCH_FLAGS = 
else ifneq (,$(findstring MSYS_NT,$(UNAME_S)))
    IS_WINDOWS = 1
    ARCH_FLAGS = 
endif

# Default Target
.DEFAULT_GOAL := all

# Dependency Paths (relative to project root)
MAGICK_PATH  = ImageMagick/install
PDF_PATH     = PDF-Writer/install
CLING_PATH   = cling/install
MISTRAL_PATH = mistral-common/install
MAPNIK_PATH  = mapnik/install
LIBZIP_PATH  = libzip/install
QUANTLIB_PATH = QuantLib/install

# Detection & Flags
CXXFLAGS = -std=c++2b -Isrc -Iccbevington/CppLibrary -O2 -Wall $(ARCH_FLAGS)
LDFLAGS = $(ARCH_FLAGS) -lpthread -lz -lreadline

ifeq ($(IS_WINDOWS),1)
    LDFLAGS += -lws2_32
    CXXFLAGS += -D_WIN32_WINNT=0x0A00
endif

# libzip Detection
ifneq ("$(wildcard $(LIBZIP_PATH)/include/zip.h)","")
    CXXFLAGS += -I$(LIBZIP_PATH)/include
    LDFLAGS  += -L$(LIBZIP_PATH)/lib -lzip
endif

# ImageMagick Detection (Strictly check install dir)
ifneq ("$(wildcard $(MAGICK_PATH)/include/ImageMagick-7/Magick++.h)","")
    CXXFLAGS += -DHAS_IMAGEMAGICK -I$(MAGICK_PATH)/include/ImageMagick-7
    CXXFLAGS += -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16
    LDFLAGS  += -L$(MAGICK_PATH)/lib -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI
    ifeq ($(UNAME_S),Darwin)
        LDFLAGS += -Wl,-rpath,$(abspath $(MAGICK_PATH)/lib)
    else ifeq ($(UNAME_S),Linux)
        LDFLAGS += -Wl,-rpath,$(abspath $(MAGICK_PATH)/lib)
    endif
endif

# PDF-Writer Detection (Strictly check install dir)
ifneq ("$(wildcard $(PDF_PATH)/include/PDFWriter/PDFWriter_DISABLED.h)","")
    CXXFLAGS += -DHAS_PDFWRITER -I$(PDF_PATH)/include/PDFWriter \
                -I$(PDF_PATH)/include/FreeType/include \
                -I$(PDF_PATH)/include/LibAesgm \
                -I$(PDF_PATH)/include/LibJpeg \
                -I$(PDF_PATH)/include/LibPng \
                -I$(PDF_PATH)/include/LibTiff \
                -I$(PDF_PATH)/include/Zlib
    LDFLAGS  += -L$(PDF_PATH)/lib -lPDFWriter -lFreeType -lLibJpeg -lLibPng -lLibTiff -lZlib
endif

# Cling Detection
ifneq ("$(wildcard $(CLING_PATH)/include/cling/Interpreter/Interpreter.h)","")
    CXXFLAGS += -I$(CLING_PATH)/include
    LDFLAGS  += -L$(CLING_PATH)/lib -lclingInterpreter -lclingUtils
endif

# Mistral Detection
ifneq ("$(wildcard $(MISTRAL_PATH)/include/mistral/mistral.h)","")
    CXXFLAGS += -I$(MISTRAL_PATH)/include
    LDFLAGS  += -L$(MISTRAL_PATH)/lib -lmistral
endif

# Mapnik Detection
ifneq ("$(wildcard $(MAPNIK_PATH)/include/mapnik/map.hpp)","")
    CXXFLAGS += -DHAS_MAPNIK -I$(MAPNIK_PATH)/include
    LDFLAGS  += -L$(MAPNIK_PATH)/lib -lmapnik
endif

# QuantLib Detection
QUANTLIB_PATH = QuantLib/install
BREW_QL_PATH  = $(shell brew --prefix quantlib 2>/dev/null)

ifneq ("$(wildcard $(QUANTLIB_PATH)/include/ql/quantlib.hpp)","")
    CXXFLAGS += -DHAS_QUANTLIB -I$(QUANTLIB_PATH)/include
    LDFLAGS  += -L$(QUANTLIB_PATH)/lib -lQuantLib
    ifneq ($(UNAME_S),Windows_NT)
        LDFLAGS += -Wl,-rpath,$(abspath $(QUANTLIB_PATH)/lib)
    endif
else ifneq ($(BREW_QL_PATH),)
ifneq ("$(wildcard $(BREW_QL_PATH)/include/ql/quantlib.hpp)","")
    CXXFLAGS += -DHAS_QUANTLIB -I$(BREW_QL_PATH)/include
    LDFLAGS  += -L$(BREW_QL_PATH)/lib -lQuantLib
    ifneq ($(UNAME_S),Windows_NT)
        LDFLAGS += -Wl,-rpath,$(BREW_QL_PATH)/lib
    endif
endif
endif

# Python Integration
ifeq ($(IS_WINDOWS),1)
    PYTHON_INC := $(shell python3 -c "import sysconfig; print('-I' + sysconfig.get_path('include'))" 2>/dev/null || python -c "import sysconfig; print('-I' + sysconfig.get_path('include'))" 2>/dev/null)
    PYTHON_LIB := $(shell python3 -c "import sysconfig; import os; print('-L' + sysconfig.get_config_var('LIBDIR').replace(os.sep, '/') + ' -lpython' + sysconfig.get_config_var('LDVERSION'))" 2>/dev/null || python -c "import sysconfig; import os; print('-L' + sysconfig.get_config_var('LIBDIR').replace(os.sep, '/') + ' -lpython' + sysconfig.get_config_var('LDVERSION'))" 2>/dev/null)
else
    HAS_PYCONFIG := $(shell command -v python3-config 2> /dev/null)
    ifdef HAS_PYCONFIG
        PYTHON_INC = $(shell python3-config --cflags | sed 's/-arch [^ ]*//g')
        PYTHON_LIB = $(shell python3-config --ldflags --embed | sed 's/-arch [^ ]*//g')
    else
        PYTHON_INC = $(shell python3 -c "import sysconfig; print('-I' + sysconfig.get_path('include'))")
        PYTHON_LIB = $(shell python3 -c "import sysconfig; import os; print('-L' + sysconfig.get_config_var('LIBDIR') + ' -lpython' + sysconfig.get_config_var('LDVERSION'))")
    endif
endif

# Sources
GEM_SRCS   = src/main.cpp src/gem_mistral_bridge.cpp
MINIZ_SRCS = src/miniz.c src/miniz_tdef.c src/miniz_tinfl.c src/miniz_zip.c
MINIZ_OBJS = $(MINIZ_SRCS:.c=.o)
BEV_SRCS   = ccbevington/CppLibrary/FitUtil.cpp ccbevington/CppLibrary/GenUtil.cpp ccbevington/CppLibrary/MonteLib.cpp ccbevington/CppLibrary/QuikHist.cpp

.PHONY: all build deps clean help clean-deps test install docs update-docs mindmap

# Docs: stamp markdown with current version/date, then regenerate LaTeX + PDF
# Requires: pandoc, MacTeX/TeX Live (xelatex)
GEM_VERSION   = 0.1.0
GEM_DATE      = 2026-04-16
PANDOC        = pandoc
PANDOC_ENGINE = xelatex
DOCSDIR       = docs
PANDOC_FLAGS  = --pdf-engine=$(PANDOC_ENGINE) --include-in-header=gem_latex_header.tex \
                -V author="David B Hon" -V date="$(GEM_DATE)"
# Prepend common TeX binary locations so xelatex is found regardless of shell PATH
TEX_PATHS     = /Library/TeX/texbin:/usr/local/texlive/bin/x86_64-linux:/usr/bin
export PATH   := $(TEX_PATHS):$(PATH)

# Update version/date stamp in both markdown files
update-docs:
	@sed -i.bak 's/\*\*Version:\*\* [0-9.]* | \*\*Date:\*\* [0-9-]*/\*\*Version:\*\* $(GEM_VERSION) | \*\*Date:\*\* $(GEM_DATE)/' README.md tutorial/README.md
	@rm -f README.md.bak tutorial/README.md.bak
	@echo "Stamped README.md and tutorial/README.md with v$(GEM_VERSION) / $(GEM_DATE)"

docs: update-docs mindmap $(DOCSDIR)/gem_lang_reference.pdf $(DOCSDIR)/gem_lang_tutorials.pdf

mindmap: $(DOCSDIR)/gem_mindmap.svg $(DOCSDIR)/gem_mindmap.png
	@echo "Mindmap → $(DOCSDIR)/gem_mindmap.svg  $(DOCSDIR)/gem_mindmap.png"

$(DOCSDIR)/gem_mindmap.svg: $(DOCSDIR)/gem_mindmap.md src/gem_base.hpp tutorial/41_art.g
	@mkdir -p $(DOCSDIR)
	./$(TARGET) tutorial/41_art.g 2>/dev/null || true
	@# Regenerate interactive SVG: pipe mermaid mindmap through mmdc if available
	@if command -v mmdc >/dev/null 2>&1; then \
	  mmdc -i $(DOCSDIR)/gem_mindmap.md -o $(DOCSDIR)/gem_mindmap.svg; \
	elif command -v npx >/dev/null 2>&1; then \
	  npx --yes @mermaid-js/mermaid-cli -i $(DOCSDIR)/gem_mindmap.md -o $(DOCSDIR)/gem_mindmap.svg; \
	else \
	  echo "Warning: mmdc not found — SVG not regenerated from mermaid source"; \
	fi
	@touch $(DOCSDIR)/gem_mindmap.svg

$(DOCSDIR)/gem_mindmap.png: $(DOCSDIR)/gem_mindmap.svg
	rsvg-convert -w 2400 -h 2000 $(DOCSDIR)/gem_mindmap.svg -o $(DOCSDIR)/gem_mindmap.png
	@echo "PNG → $(DOCSDIR)/gem_mindmap.png (2400x2000)"

$(DOCSDIR)/gem_lang_reference.pdf $(DOCSDIR)/gem_lang_reference.tex: README.md gem_latex_header.tex
	@mkdir -p $(DOCSDIR)
	$(PANDOC) README.md $(PANDOC_FLAGS) -V title="Gem Language Reference" -s -o $(DOCSDIR)/gem_lang_reference.tex
	$(PANDOC) README.md $(PANDOC_FLAGS) -V title="Gem Language Reference"    -o $(DOCSDIR)/gem_lang_reference.pdf

$(DOCSDIR)/gem_lang_tutorials.pdf $(DOCSDIR)/gem_lang_tutorials.tex: tutorial/README.md gem_latex_header.tex
	@mkdir -p $(DOCSDIR)
	$(PANDOC) tutorial/README.md $(PANDOC_FLAGS) -V title="Gem Language Tutorial Series" -s -o $(DOCSDIR)/gem_lang_tutorials.tex
	$(PANDOC) tutorial/README.md $(PANDOC_FLAGS) -V title="Gem Language Tutorial Series"    -o $(DOCSDIR)/gem_lang_tutorials.pdf

gem_latex_header.tex:
	@printf '%s\n' \
	  '\usepackage{fancyhdr}' \
	  '\usepackage{eso-pic}' \
	  '\usepackage{tikz}' \
	  '\usepackage{geometry}' \
	  '\usepackage{xcolor}' \
	  '\geometry{margin=1in}' \
	  '\AddToShipoutPictureBG{%' \
	  '  \begin{tikzpicture}[remember picture, overlay]' \
	  '    \node[rotate=45, scale=3.5, text opacity=0.10, color=gray]' \
	  '      at (current page.center)' \
	  '      {\textsf{Gem v$(GEM_VERSION) \textperiodcentered\ $(GEM_DATE)}};' \
	  '  \end{tikzpicture}%' \
	  '}' \
	  '\pagestyle{fancy}' \
	  '\fancyhf{}' \
	  '\fancyhead[L]{\small\textbf{Gem Language} v$(GEM_VERSION)}' \
	  '\fancyhead[R]{\small $(GEM_DATE)}' \
	  '\fancyfoot[C]{\small\thepage}' \
	  '\renewcommand{\headrulewidth}{0.4pt}' \
	  '\renewcommand{\footrulewidth}{0.4pt}' \
	  > gem_latex_header.tex

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib/gem
DATADIR = $(PREFIX)/share/gem

all:
	@$(MAKE) clean
	@$(MAKE) build

ifeq ($(IS_WINDOWS),1)
    GEM_TEST_TARGET = gem_test.exe
else
    GEM_TEST_TARGET = gem_test
endif

build: $(TARGET) $(GEM_TEST_TARGET)

install: build
	@echo "Installing Gem Language to $(PREFIX)..."
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(DATADIR)
	install -m 755 $(TARGET) $(BINDIR)/
	install -m 755 $(GEM_TEST_TARGET) $(BINDIR)/
	# Install root modules
	find . -maxdepth 1 -name "*.gm" -exec cp {} $(LIBDIR)/ \;
	# Install package modules (bsm, etc.)
	mkdir -p $(LIBDIR)/bsm
	cp bsm/*.gm $(LIBDIR)/bsm/ 2>/dev/null || true
	# Install tutorials and tests to share
	cp -r tutorial $(DATADIR)/
	cp -r tests $(DATADIR)/
	@echo "Installation complete. Add $(BINDIR) to your PATH if it is not already there."

help:
	@echo "Gem Language Build System"
	@echo "Targets:"
	@echo "  all        - Perform a clean compile of gem interpreter and test suite (default)"
	@echo "  build      - Build without cleaning (used internally by 'all')"
	@echo "  $(GEM_TEST_TARGET)   - Build the comprehensive test suite binary"
	@echo "  deps       - Clone and build all dependencies"
	@echo "  install    - Install binaries and modules into $(PREFIX)"
	@echo "  clean      - Remove built binaries"
	@echo "  clean-deps - Remove all dependency source folders"
	@echo "  docs       - Stamp markdown version/date, regenerate LaTeX + PDF docs"
	@echo "  help       - Show this help message"

src/gem_mistral_bridge.cpp: src/gem_mistral_bridge.pyx
	@echo "Generating C++ from Cython..."
	@cython --cplus -3 src/gem_mistral_bridge.pyx -o src/gem_mistral_bridge.cpp

$(TARGET): $(GEM_SRCS) $(MINIZ_OBJS) $(BEV_SRCS)
	@echo "Building $(TARGET) via $(CXX) $(CXXFLAGS) $(PYTHON_INC) $(ARCH_FLAGS)"
	$(CXX) $(CXXFLAGS) $(PYTHON_INC) $(ARCH_FLAGS) -o $(TARGET) $(GEM_SRCS) $(MINIZ_OBJS) $(BEV_SRCS) $(LDFLAGS) $(PYTHON_LIB)

# Compile miniz C files as plain C (not C++) to avoid deprecation warnings
src/%.o: src/%.c
	$(CC) -c -O2 $(ARCH_FLAGS) -Isrc -o $@ $<

$(GEM_TEST_TARGET): $(TARGET) tests/comprehensive/all_tests.g
	@echo "Building comprehensive test suite binary ($(GEM_TEST_TARGET))..."
	./$(TARGET) -c tests/comprehensive/all_tests.g -o $(GEM_TEST_TARGET)

test: $(GEM_TEST_TARGET)
	@echo "Running comprehensive tests..."
	./$(GEM_TEST_TARGET)
	@echo ""
	@echo "Running help tests..."
	./$(TARGET) tests/test_help.g

test-async: $(TARGET)
	@echo "Running async builtin tests..."
	./$(TARGET) test_async.g

deps:
	@echo "Cloning and building dependencies (errors ignored)..."
	-@if [ ! -d "libzip" ]; then \
		git clone --depth=1 https://github.com/nih-at/libzip.git && \
		cd libzip && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(abspath $(LIBZIP_PATH)) -DBUILD_SHARED_LIBS=OFF -DENABLE_GNUTLS=OFF -DENABLE_MBEDTLS=OFF -DENABLE_OPENSSL=OFF -DENABLE_COMMONCRYPTO=OFF .. && make -j4 && make install; \
	fi
	-@if [ ! -d "ImageMagick" ]; then \
		git clone --depth=1 https://github.com/ImageMagick/ImageMagick.git && \
		cd ImageMagick && ./configure --prefix=$(abspath $(MAGICK_PATH)) --with-magick-plus-plus=yes --disable-docs --disable-openmp AX_CV_PROG_CC_OPENMP=no AX_CV_PROG_CXX_OPENMP=no CFLAGS="$(ARCH_FLAGS)" CXXFLAGS="$(ARCH_FLAGS)" LDFLAGS="$(ARCH_FLAGS)" && make -j4 && make install; \
	fi
	-@if [ ! -d "PDF-Writer" ]; then \
		git clone --depth=1 https://github.com/galkahana/PDF-Writer.git && \
		cd PDF-Writer && cmake -DCMAKE_INSTALL_PREFIX=$(abspath $(PDF_PATH)) -DCMAKE_OSX_ARCHITECTURES=arm64 . && make -j4 && make install; \
	fi
	-@if [ ! -d "cling" ]; then \
		git clone --depth=1 https://github.com/root-project/cling.git && \
		cd cling && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(abspath $(CLING_PATH)) -DCMAKE_OSX_ARCHITECTURES=arm64 -DCLING_CXX_PATH=g++ .. && make -j4 && make install; \
	fi
	-@if [ ! -d "mistral-common" ]; then \
		git clone --depth=1 https://github.com/mistralai/mistral-common.git && \
		cd mistral-common && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(abspath $(MISTRAL_PATH)) -DCMAKE_OSX_ARCHITECTURES=arm64 .. && make -j4 && make install; \
	fi
	-@if [ ! -d "mapnik" ]; then \
		git clone --depth=1 https://github.com/mapnik/mapnik.git && \
		cd mapnik && git submodule update --init --recursive --depth 1 && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(abspath $(MAPNIK_PATH)) -DCMAKE_OSX_ARCHITECTURES=arm64 .. && make -j4 && make install; \
	fi
	-@if [ ! -d "cython" ]; then git clone --depth=1 https://github.com/cython/cython.git; fi
	-@if [ ! -d "mistral-client-python" ]; then git clone --depth=1 https://github.com/mistralai/client-python.git mistral-client-python; fi
	-@if [ ! -d "mistral-inference" ]; then git clone --depth=1 https://github.com/mistralai/mistral-inference.git; fi
	-@if [ ! -d "mistral-finetune" ]; then git clone --depth=1 https://github.com/mistralai/mistral-finetune.git; fi
	-@if [ ! -d "QuantLib" ]; then git clone --depth=1 https://github.com/lballabio/QuantLib.git && cd QuantLib && mkdir -p build && cd build && \
		cmake -DCMAKE_INSTALL_PREFIX=$(abspath $(QUANTLIB_PATH)) -DCMAKE_CXX_STANDARD=20 -DQL_BUILD_EXAMPLES=OFF -DQL_BUILD_TEST_SUITE=OFF -DQL_BUILD_BENCHMARK=OFF .. && \
		make -j4 && make install; \
	fi

clean:
	#$(RM) $(TARGET) $(TARGET).exe node_modules gem_test gem_test.exe $(MINIZ_OBJS) # src/gem_mistral_bridge.cpp
	$(RM) node_modules gem_test $(MINIZ_OBJS) # src/gem_mistral_bridge.cpp
	# Clean up any other temporary build files if necessary
	# find . -maxdepth 1 -type f ! -name "*.sh" ! -name "Makefile" ! -name "*.py" ! -name "*.jl" ! -name "*.md" ! -name "*.g" ! -name "*.gm" -delete 2>/dev/null || true

clean-deps:
	$(RM) ImageMagick PDF-Writer QuantLib cling mapnik mistral-common cython mistral-client-python mistral-inference mistral-finetune mistral-vibe
	@echo deps cleaned
