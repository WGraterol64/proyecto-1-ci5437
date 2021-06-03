CC = gcc
CXX = g++
OPT1 = -g -Wall -O3
OPT2 = -Wall -O3 -Wno-unused-function -Wno-unused-variable
PSVNOPT = --no_state_map --no_abstraction --no_backwards_moves --history_len=0

%.out: %.c src/*.cpp
	cd src; $(CXX) $(OPT2) *.cpp -include $< -o ../bin/$@ || rm -rf *.c
	rm -f src/$*.c
	
%.c: %.psvn psvn/bin/psvn2c psvn2c_core.c psvn2c_state_map.c psvn2c_abstraction.c
	cd src; ../psvn/bin/psvn2c $(PSVNOPT) --name=$(*F) < ../$< > $@
	rm -f src/psvn2c_core.c src/psvn2c_state_map.c src/psvn2c_abstraction.c
	
psvn/bin/psvn2c: psvn/src/psvn2c.cpp psvn/src/psvn2c.hpp psvn/src/psvn.cpp psvn/src/psvn.hpp
	$(CXX) $(OPT1) psvn/src/psvn2c.cpp psvn/src/psvn.cpp -o $@

psvn2c_core.c:
	cp psvn/src/psvn2c_core.c src/psvn2c_core.c

psvn2c_state_map.c:
	cp psvn/src/psvn2c_state_map.c src/psvn2c_state_map.c

psvn2c_abstraction.c:
	cp psvn/src/psvn2c_abstraction.c src/psvn2c_abstraction.c

.PRECIOUS: %.c

