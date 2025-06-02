TARGET=ui-game
CC=gcc
DEBUG=-g
OPT=-O0
WARN=-Wall
CURSES=-lncurses
TINFO=-ltinfo
CCFLAGS=$(DEBUG) $(OPT) $(WARN)
LD=gcc
OBJS=ui-game.c
all: ui-game.c
	$(LD) -o $(TARGET) $(OBJS) $(DEBUG) $(OPT) $(WARN) $(CURSES) $(TINFO)
