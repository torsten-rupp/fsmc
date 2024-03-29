# ----------------------------------------------------------------------
# build finite state machine compiler
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------

# disable implicit rule for *.[yl] -> *.[ch]
%.c: %.y
%.c: %.l

CFLAGS   = -std=c99 -g -D_GNU_SOURCE -I. -O0
CXXFLAGS = -std=c++11 -g -D_GNU_SOURCE -I. -g -O0
LDFLAGS  = -g

%.o:%.c
	gcc $(CFLAGS) -c $*.c -o $@
%.o:%.cpp
	g++ $(CXXFLAGS) -c $*.cpp -o $@

# ----------------------------------------------------------------------

.PHONY: default
default: all

.PHONY: help
help:
	@echo "Help:"
	@echo ""
	@echo "docker_update   build/update docker build container"
	@echo "docker_run      run docker build container"
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

# ----------------------------------------------------------------------

.PHONY: docker_update docker_run
docker_update:
	docker build -f docker/build.dockerfile -t fsmc docker
docker_run:
	docker run -i -t --rm --net=host -v $(HOME):$(HOME) -w $(PWD) -u $(shell id -u):$(shell id -g) fsmc /bin/bash

# ----------------------------------------------------------------------

scanner.cpp: scanner.l location.h
	flex -o scanner.cpp scanner.l
parser.tab.c parser.tab.h: parser.y location.h
	byacc -B -b parser -d -t parser.y || (echo rm -f parser.tab.c parser.tab.h && false)

fsmc.o: fsmc.cpp scanner.h parser.h ast.h parser.tab.h
	g++ $(CXXFLAGS) -DGIT_HASH=$(shell git rev-parse HEAD) -std=c++11 -c -g $*.cpp -o $@
scanner.o: scanner.cpp scanner.h parser.tab.h
parser.o: parser.cpp parser.h scanner.h ast.h parser.tab.c parser.tab.h
ast.o: ast.cpp ast.h visitor.h
visitor.o: visitor.cpp visitor.h
codegenerator.o: codegenerator.cpp codegenerator.h ast.h visitor.h
dotgenerator.o: dotgenerator.cpp dotgenerator.h ast.h visitor.h
fsmc: fsmc.o scanner.o parser.o ast.o visitor.o codegenerator.o dotgenerator.o
	g++ $(LDFLAGS) $^ -o fsmc

# ----------------------------------------------------------------------

.PHONY: tests
tests:
	$(MAKE) -C tests

test%:
	$(MAKE) -C tests test$*

# ----------------------------------------------------------------------

.PHONY: examples
examples:
	$(MAKE) -C examples

# user makefile
-include Makefile.user
