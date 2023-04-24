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
	rm -f parser.tab.c parser.tab.h parser.output
	rm -f location.hh position.hh stack.hh
	rm -f fsmc.o scanner.o parser.o ast.o
	rm -f codegenerator.o dotgenerator.o
	rm -f fsmc
	$(MAKE) -C tests clean
	$(MAKE) -C examples clean

.PHONY: docker_update docker_run
docker_update:
	docker build -f docker/build.dockerfile -t fsmc docker
docker_run:
	docker run -i -t --rm --net=host -v $(HOME):$(HOME) -w $(PWD) -u $(shell id -u):$(shell id -g) fsmc /bin/bash

scanner.cpp: scanner.l
	flex -o scanner.cpp scanner.l
parser.tab.c parser.tab.h: parser.y
	byacc -B -b parser -d -t parser.y
#	byacc -b parser -d -t parser.y

fsmc.o: fsmc.cpp scanner.h parser.h ast.h parser.tab.h
	g++ $(CXXFLAGS) -DGIT_HASH=$(shell git rev-parse HEAD) -std=c++11 -c -g $*.cpp -o $@
scanner.o: scanner.cpp scanner.h parser.tab.h
parser.o: parser.cpp parser.h scanner.h ast.h parser.tab.c parser.tab.h
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
