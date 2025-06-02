# Executable name
TARGET=out/dnt-rpg

# Compiler and linker
CC=gcc
LD=gcc

# Debugging, Optimization, and Warning flags
DEBUG=-g
OPT=-O0
WARN=-Wall
CCFLAGS=$(DEBUG) $(OPT) $(WARN)

# Libraries (add here more if needed)
CURSES=-lncurses
TINFO=-ltinfo
LIBS=$(CURSES) $(TINFO) -Iinclude

# Modules
CLASSES=lib/classes.c
MODULES=$(CLASSES)

OBJS=src/main.c
all: src/main.c
	$(LD) -o $(TARGET) $(MODULES) $(OBJS) $(DEBUG) $(OPT) $(WARN) $(LIBS)
