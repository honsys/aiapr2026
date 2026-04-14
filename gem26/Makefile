# Gem Language Makefile
# High-performance STEM language implemented in C++26

CXX = g++
TARGET = gem
RM = rm -f

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

# Dependency Paths
PWD = $(shell pwd)
MAGICK_PATH = $(PWD)/ImageMagick/install
PDF_PATH    = $(PWD)/PDF-Writer/install
CLING_PATH  = $(PWD)/cling/install
MISTRAL_PATH = $(PWD)/mistral-common/install
MAPNIK_PATH = $(PWD)/mapnik/install
LIBZIP_PATH = $(PWD)/libzip/install

# Detection & Flags
CXXFLAGS = -std=c++2b -Igem_src -Iccbevington/CppLibrary -O2 -Wall $(ARCH_FLAGS)
LDFLAGS = $(ARCH_FLAGS) -lpthread -lz -lreadline

ifeq ($(IS_WINDOWS),1)
    LDFLAGS += -lws2_32
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
QUANTLIB_PATH = $(PWD)/QuantLib/install
BREW_QL_PATH = /usr/local/opt/quantlib
BREW_QL_ARM_PATH = /opt/homebrew/opt/quantlib

ifneq ("$(wildcard $(QUANTLIB_PATH)/include/ql/quantlib.hpp)","")
    CXXFLAGS += -DHAS_QUANTLIB -I$(QUANTLIB_PATH)/include
    LDFLAGS  += -L$(QUANTLIB_PATH)/lib -lQuantLib
else ifneq ("$(wildcard $(BREW_QL_PATH)/include/ql/quantlib.hpp)","")
    CXXFLAGS += -DHAS_QUANTLIB -I$(BREW_QL_PATH)/include
    LDFLAGS  += -L$(BREW_QL_PATH)/lib -lQuantLib
else ifneq ("$(wildcard $(BREW_QL_ARM_PATH)/include/ql/quantlib.hpp)","")
    CXXFLAGS += -DHAS_QUANTLIB -I$(BREW_QL_ARM_PATH)/include
    LDFLAGS  += -L$(BREW_QL_ARM_PATH)/lib -lQuantLib
endif

# Python Integration
HAS_PYCONFIG := $(shell command -v python3-config 2> /dev/null)
ifeq ($(IS_WINDOWS),1)
    ifndef HAS_PYCONFIG
        PYTHON_PREFIX := $(shell python -c "import sys; import os; print(sys.base_prefix.replace(os.sep, '/'))")
        PYTHON_VERSION := $(shell python -c "import sys; print(f'{sys.version_info.major}{sys.version_info.minor}')")
        PYTHON_INC = -I$(PYTHON_PREFIX)/include
        PYTHON_LIB = -L$(PYTHON_PREFIX)/libs -lpython$(PYTHON_VERSION)
    else
        PYTHON_INC = $(shell python3-config --cflags | sed 's/-arch [^ ]*//g')
        PYTHON_LIB = $(shell python3-config --ldflags --embed | sed 's/-arch [^ ]*//g')
    endif
else
    PYTHON_INC = $(shell python3-config --cflags | sed 's/-arch arm64//g' | sed 's/-arch x86_64//g')
    PYTHON_LIB = $(shell python3-config --ldflags --embed | sed 's/-arch arm64//g' | sed 's/-arch x86_64//g')
endif

# Sources
GEM_SRCS = gem_src/main.cpp gem_src/gem_mistral_bridge.cpp \
           gem_src/miniz.c gem_src/miniz_tdef.c gem_src/miniz_tinfl.c gem_src/miniz_zip.c
BEV_SRCS = ccbevington/CppLibrary/FitUtil.cpp ccbevington/CppLibrary/GenUtil.cpp ccbevington/CppLibrary/MonteLib.cpp ccbevington/CppLibrary/QuikHist.cpp

.PHONY: all build deps clean help clean-deps test install

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib/gem
DATADIR = $(PREFIX)/share/gem

all:
	@$(MAKE) clean
	@$(MAKE) build

build: $(TARGET) gem_test

install: build
	@echo "Installing Gem Language to $(PREFIX)..."
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(DATADIR)
	install -m 755 $(TARGET) $(BINDIR)/
	install -m 755 gem_test $(BINDIR)/
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
	@echo "  gem_test   - Build the comprehensive test suite binary"
	@echo "  deps       - Clone and build all dependencies"
	@echo "  install    - Install binaries and modules into $(PREFIX)"
	@echo "  clean      - Remove built binaries"
	@echo "  clean-deps - Remove all dependency source folders"
	@echo "  help       - Show this help message"

gem_src/gem_mistral_bridge.cpp: gem_src/gem_mistral_bridge.pyx
	@echo "Generating C++ from Cython..."
	@cython --cplus -3 gem_src/gem_mistral_bridge.pyx -o gem_src/gem_mistral_bridge.cpp

$(TARGET): $(GEM_SRCS) $(BEV_SRCS)
	@echo "Building $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(PYTHON_INC) $(ARCH_FLAGS) -o $(TARGET) $(GEM_SRCS) $(BEV_SRCS) $(LDFLAGS) $(PYTHON_LIB)

gem_test: $(TARGET) tests/comprehensive/all_tests.g
	@echo "Building comprehensive test suite binary (gem_test)..."
	./$(TARGET) -c tests/comprehensive/all_tests.g -o gem_test

test: gem_test
	@echo "Running comprehensive tests..."
	./gem_test

test-async: $(TARGET)
	@echo "Running async builtin tests..."
	./$(TARGET) test_async.g

deps:
	@echo "Cloning and building dependencies (errors ignored)..."
	-@if [ ! -d "libzip" ]; then \
		git clone --depth=1 https://github.com/nih-at/libzip.git && \
		cd libzip && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(LIBZIP_PATH) -DBUILD_SHARED_LIBS=OFF -DENABLE_GNUTLS=OFF -DENABLE_MBEDTLS=OFF -DENABLE_OPENSSL=OFF -DENABLE_COMMONCRYPTO=OFF .. && make -j4 && make install; \
	fi
	-@if [ ! -d "ImageMagick" ]; then \
		git clone --depth=1 https://github.com/ImageMagick/ImageMagick.git && \
		cd ImageMagick && ./configure --prefix=$(MAGICK_PATH) --with-magick-plus-plus=yes --disable-docs --disable-openmp AX_CV_PROG_CC_OPENMP=no AX_CV_PROG_CXX_OPENMP=no CFLAGS="$(ARCH_FLAGS)" CXXFLAGS="$(ARCH_FLAGS)" LDFLAGS="$(ARCH_FLAGS)" && make -j4 && make install; \
	fi
	-@if [ ! -d "PDF-Writer" ]; then \
		git clone --depth=1 https://github.com/galkahana/PDF-Writer.git && \
		cd PDF-Writer && cmake -DCMAKE_INSTALL_PREFIX=$(PDF_PATH) -DCMAKE_OSX_ARCHITECTURES=arm64 . && make -j4 && make install; \
	fi
	-@if [ ! -d "cling" ]; then \
		git clone --depth=1 https://github.com/root-project/cling.git && \
		cd cling && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(CLING_PATH) -DCMAKE_OSX_ARCHITECTURES=arm64 -DCLING_CXX_PATH=g++ .. && make -j4 && make install; \
	fi
	-@if [ ! -d "mistral-common" ]; then \
		git clone --depth=1 https://github.com/mistralai/mistral-common.git && \
		cd mistral-common && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(MISTRAL_PATH) -DCMAKE_OSX_ARCHITECTURES=arm64 .. && make -j4 && make install; \
	fi
	-@if [ ! -d "mapnik" ]; then \
		git clone --depth=1 https://github.com/mapnik/mapnik.git && \
		cd mapnik && git submodule update --init --recursive --depth 1 && mkdir -p build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(MAPNIK_PATH) -DCMAKE_OSX_ARCHITECTURES=arm64 .. && make -j4 && make install; \
	fi
	-@if [ ! -d "cython" ]; then git clone --depth=1 https://github.com/cython/cython.git; fi
	-@if [ ! -d "mistral-client-python" ]; then git clone --depth=1 https://github.com/mistralai/client-python.git mistral-client-python; fi
	-@if [ ! -d "mistral-inference" ]; then git clone --depth=1 https://github.com/mistralai/mistral-inference.git; fi
	-@if [ ! -d "mistral-finetune" ]; then git clone --depth=1 https://github.com/mistralai/mistral-finetune.git; fi
	-@if [ ! -d "QuantLib" ]; then \
		git clone --depth=1 https://github.com/lballabio/QuantLib.git && \
		cd QuantLib && mkdir -p build && cd build && \
		cmake -DCMAKE_INSTALL_PREFIX=$(QUANTLIB_PATH) -DCMAKE_CXX_STANDARD=20 -DQL_BUILD_EXAMPLES=OFF -DQL_BUILD_TEST_SUITE=OFF -DQL_BUILD_BENCHMARK=OFF .. && \
		make -j4 && make install; \
	fi

clean:
	$(RM) $(TARGET) node_modules gem_test gem_src/gem_mistral_bridge.cpp
	@# Remove any other potential Gem-compiled binaries in the root
	@find . -maxdepth 1 -type f -perm +111 ! -name "*.sh" ! -name "Makefile" ! -name "*.py" ! -name "*.jl" -delete

clean-deps:
	rm -rf ImageMagick PDF-Writer cling mistral-common cython mistral-client-python mistral-inference mistral-finetune ma
