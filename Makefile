# ----------------------------------------------------------------------
# build finite state machine compiler
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------

# disable implicit rule for *.[yl] -> *.[ch]
%.c: %.y
%.c: %.l

CFLAGS   = -D_GNU_SOURCE -I.
CXXFLAGS = -D_GNU_SOURCE -I.

%.o:%.c
	gcc $(CFLAGS) -std=c99 -c -g $*.c -o $@
%.o:%.cpp
	g++ $(CXXFLAGS) -std=c++11 -c -g $*.cpp -o $@

# ----------------------------------------------------------------------

.PHONY: default
default: all

.PHONY: help
help:
	@echo "Help:"
	@echo ""
	@echo "make fsmc"
	@echo "make clean"
	@echo ""
	@echo "make tests"
	@echo ""
	@echo "make examples"

.PHONY: all
all: fsmc

.PHONY: clean
clean:
	rm -f scanner.cpp
	rm -f parser.cpp parser.hpp parser.output location.hh position.hh stack.hh
	rm -f fsmc.o scanner.o parser.o ast.o
	rm -f codegenerator.o dotgenerator.o
	rm -f fsmc
	$(MAKE) -C tests clean
	$(MAKE) -C examples clean

scanner.cpp: scanner.l
	flex -o scanner.cpp scanner.l
parser.cpp parser.hpp: parser.y
	bison -o parser.cpp parser.y

fsmc.o: fsmc.cpp scanner.h parser.hpp ast.h
scanner.o: scanner.cpp scanner.h parser.hpp
parser.o: parser.cpp parser.hpp scanner.h ast.h
ast.o: ast.cpp ast.h visitor.h
visitor.o: visitor.cpp visitor.h
codegenerator.o: codegenerator.cpp codegenerator.h ast.h visitor.h
dotgenerator.o: dotgenerator.cpp dotgenerator.h ast.h visitor.h

fsmc: fsmc.o scanner.o parser.o ast.o visitor.o codegenerator.o dotgenerator.o
	g++ -g $^ -o fsmc

.PHONY: tests
tests:
	$(MAKE) -C tests

test%:
	$(MAKE) -C tests test$*

.PHONY: examples
examples:
	$(MAKE) -C examples

# user makefile
-include Makefile.user
