# List source files (not headers!) and binary name:
SOURCES=$(wildcard *.cc) ../lts_draw_write.cc
BINARY=test

# Set up compiler and compiler options:
CXX=g++
CXXFLAGS=-Wall -W -ansi -O3
LDLIBS=

# If you are compiling a C program (instead of C++), comment out this line:
LINK.o=$(LINK.cpp)


# --- No need to modify anything below this line ---
$(BINARY): $(SOURCES:.cc=.o)

.dep:
	g++ -MM $(SOURCES) > .dep

-include .dep
