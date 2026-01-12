LIBNAME = sq3pp
VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0

BASE_TARGET = lib$(LIBNAME)-$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)


# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib

INSTALL_PREFIX?=/usr/local
ifneq ($(PREFIX),)
INSTALL_PREFIX=$(PREFIX)
endif

# Target library
SHARED?=y
DEBUG?=n
V=n

ifeq ($(V),y)
Q =
else
Q = @
endif

ifeq ($(V), 1)
Q =
endif

EXTENSION=a
ifeq ($(SHARED),y)
	EXTENSION=so
endif

TARGET = $(LIB_DIR)/$(BASE_TARGET).$(EXTENSION)
SYM_LINK_TARGET = $(LIB_DIR)/lib$(LIBNAME).$(EXTENSION)

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS=
LIBS = -lsqlite3

ifeq ($(SHARED),y)
LINKER=$(CXX)
CXXFLAGS += -fPIC
LDFLAGS += -shared -Wl,--no-as-needed $(LIBS) -Wl,--no-as-needed -o
else
LINKER=ar
LDFLAGS += rcs
endif

ifeq ($(DEBUG),y)
CXXFLAGS += -g -O0
else 
CXXFLAGS += -O3
endif

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Default target
all: $(SYM_LINK_TARGET) examples

# Create necessary directories
$(OBJ_DIR):
	$(Q)mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	$(Q)mkdir -p $(LIB_DIR)

$(SYM_LINK_TARGET): $(TARGET)
	$(Q)ln -sf $(notdir $(TARGET)) $@

# Link object files to create executable
$(TARGET): $(OBJECTS) | $(LIB_DIR)
	@echo "LINK $@"
	$(Q)$(LINKER) $(LDFLAGS) $@ $(OBJECTS)
	

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "CXX $<"
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "CLEAN $(BUILD_DIR)"
	$(Q)rm -rf $(BUILD_DIR)


install: all
	$(Q)mkdir -p $(INSTALL_PREFIX)/lib
	@cp -v $(TARGET) $(INSTALL_PREFIX)/lib/
	@ln -sf $(notdir $(TARGET)) $(INSTALL_PREFIX)/lib/lib$(LIBNAME).$(EXTENSION)

	$(Q)mkdir -p $(INSTALL_PREFIX)/include/sq3pp
	$(Q)cp -arv $(INCLUDE_DIR)/sq3pp/* $(INSTALL_PREFIX)/include/sq3pp
	@echo "Installed to $(INSTALL_PREFIX)"

examples: $(SYM_LINK_TARGET)
	@echo "Build $(BIN_DIR)/example"
	$(Q)mkdir -p $(BIN_DIR)
	$(Q)g++ -std=c++17 -Iinclude example/main.cpp -o $(BIN_DIR)/example -L$(LIB_DIR) -lsq3pp -lsqlite3 -Wl,-rpath=$(LIB_DIR)
	


.PHONY: all clean  install
