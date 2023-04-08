# ----------------------------------------------------------------------
# build finite state machine compiler tests
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------

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
	@echo "test1 [DEBUG=1]"
	@echo "test2 [DEBUG=1]"
	@echo "test3 [DEBUG=1]"
	@echo "test4 [DEBUG=1]"

.PHONY: all
all: \
  test0 \
  $(foreach i,1 2 3 4 10,test$i) \

.PHONY: clean
clean:

.PHONY: ../fsmc
../fsmc:
	$(MAKE) -C .. fsmc

.PHONY: test0
test0: ../fsmc
	echo 'f(1,2,3);'|../fsmc > /dev/null

# --- simple tests
.PHONY: test%d
test%d: ../fsmc
	gdb --args ../fsmc test$(*:%d=).c -d .

.PHONY: test%
test%: ../fsmc
	cat test$*.c|../fsmc > /dev/null

# --- tests with stack
.PHONY: test1%
test1%: ../fsmc
	../fsmc test1$*.c -d . -n 123 -a -l "logStateChange(@fromStateName@,@toStateName@)" > /dev/null

.PHONY: test1%d
test1%d: ../fsmc
	gdb --args ../fsmc test1$(*:%d=).c -d . --state-stack-size 123 --asserts --log-function "logStateChange(@fromStateName@,@toStateName@)"
