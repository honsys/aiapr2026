CXX      := g++
CXXFLAGS := -std=c++26 -O2
LDFLAGS  := -lcurl
TARGET   := geoip_last
SRC      := geoip_last.cpp

PY_SRC   := geoip_last.py
PY_BIN   := geoip_last_py

# macOS: prefer Homebrew curl headers
ifeq ($(shell uname), Darwin)
    CXXFLAGS += -I$(shell brew --prefix curl)/include
    LDFLAGS  := -L$(shell brew --prefix curl)/lib $(LDFLAGS)
endif

all: $(TARGET) $(PY_BIN)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(PY_BIN): $(PY_SRC)
	python -m nuitka --onefile --output-filename=$@ $<

clean:
	rm -f $(TARGET) $(PY_BIN)
	rm -rf $(PY_BIN).build $(PY_BIN).dist $(PY_BIN).onefile-build

.PHONY: all clean
