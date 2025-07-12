# Compiler and flags
CC       := gcc
CFLAGS   := -Iinclude -Wall -Wextra -g
LDFLAGS  := -lcurl -lcjson -Wl,-rpath,/usr/local/lib
# Directories
SRCDIR   := src
INCDIR   := include
BUILDDIR := build

# Gather source files
SRCS     := $(wildcard $(SRCDIR)/*.c)
OBJS     := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
TARGET   := $(BUILDDIR)/framework

# Default rule
all: dirs $(TARGET)

# Create build directory if missing
dirs:
	@mkdir -p $(BUILDDIR)

# Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile each .c -> .o
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


run:
	@$(BUILDDIR)/framework


# Clean up
.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*

# Rebuild from scratch
.PHONY: rebuild
rebuild: clean all