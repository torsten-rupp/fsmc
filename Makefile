# ----------------------------------------------------------------------
# build finite state machine compiler
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------

# disable implicit rule for *.[yl] -> *.[ch]
%.c: %.y
%.c: %.l

CFLAGS   = -D_GNU_SOURCE -I.
CXXFLAGS = -D_GNU_SOURCE -I.

%.o:%.cpp
	g++ $(CXXFLAGS) -std=c++11 -c -g $*.cpp -o $@

# ----------------------------------------------------------------------

.PHONY: default
default: all

.PHONY: help
help:
	@echo "Help:"
	@echo ""
	@echo "fsmc"
	@echo "test1 [DEBUG=1]"
	@echo "test2 [DEBUG=1]"
	@echo "test3 [DEBUG=1]"
	@echo "test4 [DEBUG=1]"

.PHONY: all
all: fsmc

.PHONY: clean
clean:
	rm -f scanner.cpp
	rm -f parser.cpp parser.hpp parser.output location.hh position.hh stack.hh
	rm -f main.o scanner.o parser.o ast.o
	rm -f codegenerator.o dotgenerator.o
	rm -f fsmc

scanner.cpp: scanner.l
	flex -o scanner.cpp scanner.l
parser.cpp parser.hpp: parser.y
	bison -o parser.cpp parser.y

main.o: main.cpp scanner.h parser.hpp ast.h
scanner.o: scanner.cpp scanner.h parser.hpp
parser.o: parser.cpp parser.hpp scanner.h ast.h
ast.o: ast.cpp ast.h visitor.h
visitor.o: visitor.cpp visitor.h
codegenerator.o: codegenerator.cpp codegenerator.h ast.h visitor.h
dotgenerator.o: dotgenerator.cpp dotgenerator.h ast.h visitor.h

fsmc: main.o scanner.o parser.o ast.o visitor.o codegenerator.o dotgenerator.o
	g++ -g $^ -o fsmc

.PHONY: test
test: test1

.PHONY: test1
test1: fsmc
	(echo 'f(1,2,3);'|./fsmc)

.PHONY: test2
test2: fsmc
	(cat demo.c|./fsmc)

.PHONY: test3
test3: fsmc
	(cat demo.fsm|./fsmc)

.PHONY: test4
test4: fsmc
	./fsmc demo.c -d .
.PHONY: test4d
test4d: fsmc
	gdb --args ./fsmc demo.c -d .

.PHONY: examples/traffic_lights
examples/traffic_lights: examples/traffic_lights.c fsmc
	./fsmc examples/traffic_lights.c -d examples | gcc -x c - -o examples/traffic_lights
