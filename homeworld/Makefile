PYTHON     := python3
CYTHON     := cython
BUILD_DIR  := fofworld/build

# Platform-specific shared library extension
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    EXT := .so
    CC_FLAGS := -dynamiclib
else
    EXT := .so
    CC_FLAGS := -shared
endif

PYTHON_INC := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_path('include'))")
NUMPY_INC  := $(shell $(PYTHON) -c "import numpy; print(numpy.get_include())")

# Source .py files to compile with Cython (exclude app.py / Flask entry points)
CYTHON_SRCS := fofworld/worldgen.py
CYTHON_OBJS := $(patsubst fofworld/%.py,$(BUILD_DIR)/%$(EXT),$(CYTHON_SRCS))

.PHONY: all build clean run

all: build

build: $(CYTHON_OBJS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.c: fofworld/%.py | $(BUILD_DIR)
	$(CYTHON) --3str -o $@ $<

$(BUILD_DIR)/%$(EXT): $(BUILD_DIR)/%.c
	$(CC) $(CC_FLAGS) -fPIC -O2 \
	    -I$(PYTHON_INC) -I$(NUMPY_INC) \
	    $< -o $@ \
	    $(shell $(PYTHON)-config --ldflags)

run: build
	PYTHONPATH=$(BUILD_DIR):fofworld $(PYTHON) fofworld/app.py

clean:
	rm -rf $(BUILD_DIR)
