# Gem Redirect App Makefile
# -------------------------
# Supports Gem installation, module management, and app compilation.

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib/gem
DATADIR = $(PREFIX)/share/gem

# Path to the gem interpreter
# Prefer system gem if available, otherwise look in parent directory
GEM_SYS := $(shell which gem 2>/dev/null)
GEM ?= $(if $(GEM_SYS),$(GEM_SYS),../gem)
# Gem app source
APP = url2url.g
# Compiled app name
APP_BIN = url2url
# Test script
TEST_SCRIPT = test_redirect.g

.PHONY: all build compile install test clean help

all: compile

build:
	@if [ ! -f "$(GEM)" ]; then \
		echo "Building Gem Language system from source..."; \
		cd .. && $(MAKE) all; \
	else \
		echo "Using existing Gem interpreter: $(GEM)"; \
	fi

compile: build
	@echo "Compiling Gem Redirect App (url2url.g) into standalone binary..."
	$(GEM) -c $(APP) -o $(APP_BIN)

install: all
	@echo "Installing Gem Language and App to $(PREFIX)..."
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(DATADIR)
	
	# Install Gem Interpreter
	install -m 755 $(GEM) $(BINDIR)/
	
	# Install the compiled Redirect WebApp
	install -m 755 $(APP_BIN) $(BINDIR)/
	
	# Install standard Gem modules (.gm files)
	@echo "Installing Gem modules into $(LIBDIR)..."
	# Note: using find to pick up root modules
	find .. -maxdepth 1 -name "*.gm" -exec cp {} $(LIBDIR)/ \;
	
	# Install specific sub-modules (e.g., bsm)
	mkdir -p $(LIBDIR)/bsm
	cp ../bsm/*.gm $(LIBDIR)/bsm/ 2>/dev/null || true
	
	# Install shared data (tests and tutorials) for user reference
	@echo "Installing shared documentation and tests to $(DATADIR)..."
	cp -r ../tests $(DATADIR)/
	cp -r ../tutorial $(DATADIR)/
	
	@echo "Installation successful."
	@echo "Binaries available: $(BINDIR)/gem, $(BINDIR)/$(APP_BIN)"

test: compile
	@echo "Running comprehensive Gem system tests..."
	cd .. && $(MAKE) test
	
	@echo "Running App-specific redirection logic tests..."
	$(GEM) $(TEST_SCRIPT)
	
	@echo "Verifying compiled binary exists and is executable..."
	@if [ -x $(APP_BIN) ]; then echo "Binary $(APP_BIN) is ready."; else echo "Binary check failed."; exit 1; fi
	
	@echo "--- All Tests Completed Successfully ---"

clean:
	@echo "Cleaning up build artifacts..."
	rm -f $(APP_BIN)
	cd .. && $(MAKE) clean

help:
	@echo "Gem URL Redirect App Build System"
	@echo "Available targets:"
	@echo "  all       - Build gem and compile the redirect app (default)"
	@echo "  install   - Install gem, modules, and app to $(PREFIX)"
	@echo "  test      - Run system and app-specific tests"
	@echo "  clean     - Remove compiled binaries and clean gem source"
	@echo "  compile   - Compile @url2url.g into a binary"
	@echo "  build     - Build the gem interpreter only"
