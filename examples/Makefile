# ----------------------------------------------------------------------
# build finite state machine compiler examples
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------

# disable implicit rule for *.[yl] -> *.[ch]
%.c: %.y
%.c: %.l

CFLAGS   = -D_GNU_SOURCE -I.
CXXFLAGS = -D_GNU_SOURCE -I.

%.o:%.c
	gcc $(CFLAGS) -std=c98 -c -g $*.c -o $@
%.o:%.cpp
	g++ $(CXXFLAGS) -std=c++11 -c -g $*.cpp -o $@

# ----------------------------------------------------------------------

.PHONY: default
default: all

.PHONY: help
help:
	@echo "Help:"
	@echo ""
	@echo "make traffic_lights"
	@echo ""
	@echo "make clean"

.PHONY: all
all: \
  traffic_lights

.PHONY: clean
clean:
	rm -f traffic_lights traffic_lights.o

../fsmc:
	$(MAKE) -C .. fsmc

.PHONY: traffic_lights
traffic_lights: traffic_lights.c ../fsmc
	../fsmc traffic_lights.c -d . -n 8 -a -l 'logStateChange("@fromStateName@","@toStateName@")'| gcc $(CFLAGS) -std=c99 -x c - -o traffic_lights
