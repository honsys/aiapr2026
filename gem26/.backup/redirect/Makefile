CXX = clang++
CXXFLAGS = -std=c++26 -Wall -Wextra -O2 -fPIC
LDFLAGS = -shared

# Detect OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LIB_EXT = dylib
    RPATH = -Wl,-rpath,@executable_path/.
else
    LIB_EXT = so
    RPATH = -Wl,-rpath,'$$ORIGIN'
    # Linux often needs pthread
    LDFLAGS += -lpthread
endif

LIB_NAME = www.$(LIB_EXT)
TARGET = redirect

all: $(LIB_NAME) $(TARGET)

$(LIB_NAME): www.cc www.h httplib.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) www.cc -o $(LIB_NAME)

$(TARGET): main.cc www.h $(LIB_NAME)
	$(CXX) $(CXXFLAGS) main.cc -L. -lwww -o $(TARGET) $(RPATH)

clean:
	rm -f $(LIB_NAME) $(TARGET)
