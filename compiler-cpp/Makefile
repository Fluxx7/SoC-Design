# Compiler and flags
CC := g++
CFLAGS :=  -g -gdwarf-4 -Wall
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
	$(SRC_DIR)/compiler.c \
	$(UTIL_DIR)/computils.c \
	$(PREP_DIR)/preprocessor.c \
	$(PREP_DIR)/preprocessor_utils.c \
	$(TGT_DIR)/ng-core.c

# Object and dependency files
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

# Default target
all: $(TARGET)

# Final binary
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Pattern rule for .o and .d generation together
# This compiles the .c file and also generates a .d file tracking h

# Cleanup
clean:
	rm -f $(OBJS) $(TARGET) testout.txt 

c_part:
	rm -f $(OBJS)