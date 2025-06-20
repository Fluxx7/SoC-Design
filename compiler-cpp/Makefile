# Compiler and flags
CC := g++
CFLAGS :=  -std=c++23 -g -gdwarf-4 -Wall
LDFLAGS := -lm

# Output binary
TARGET := fcc

# Source directories
SRC_DIR := src
UTIL_DIR := $(SRC_DIR)/utilities
PREP_DIR := $(SRC_DIR)/preprocessor
TGT_DIR := targets/ng-core

# Source files
SRCS := \
	$(SRC_DIR)/compiler.cpp \
	$(UTIL_DIR)/computils.cpp \
	$(PREP_DIR)/preprocessor.cpp \
	$(PREP_DIR)/preprocessor_utils.cpp \
	$(TGT_DIR)/ng-core.cpp

# Object and dependency files
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)

# Default target
all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@


# Final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Pattern rule for .o and .d generation together
# This compiles the .c file and also generates a .d file tracking h

# Cleanup
clean:
	rm -f $(OBJS) $(TARGET) testout.txt 

c_part:
	rm -f $(OBJS) $(DEPS)

-include $(DEPS)
