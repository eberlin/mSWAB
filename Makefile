# the shell to be used
SHELL = /bin/sh

# source and build directories
CURROOT=$(shell pwd)
SRCROOT=$(CURROOT)/src
OBJROOT=$(CURROOT)/build
BINROOT=$(CURROOT)/bin

# compilers and flags
CC = gcc
CFLAGS = -W -Wall -g -std=gnu99 -O3

# variables
HDRS =	$(OBJROOT)/helper.o \
		$(OBJROOT)/segm_struct.o

SEGS =	$(OBJROOT)/get_BUsegs.o \
		$(OBJROOT)/get_SWABsegs.o \
		$(OBJROOT)/get_mSWABsegs.o 

# leave this entry as first anchor!
1st:
	@if test ! -d $(BINROOT)/ ; then mkdir $(BINROOT)/ ; fi
	@if test ! -d $(OBJROOT)/ ; then mkdir $(OBJROOT)/ ; fi
	@make clean -s
	@make all -s
	@echo "done."

###########
# object files target / dependencies for gcc
$(OBJROOT)/%.o: $(SRCROOT)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


segs: $(SEGS) $(HDRS)
	$(CC) $(CFLAGS) -lm $(SRCROOT)/main_segs.c $(SEGS) $(HDRS) -o $(BINROOT)/main_segs

all:	segs 

clean:
	@rm -rf $(BINROOT)/*
	@rm -rf $(OBJROOT)/*
