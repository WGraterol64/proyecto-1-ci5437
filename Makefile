CC = gcc
CXX = g++
OPT1 = -g -Wall -O3
OPT2 = -Wall -O3 -Wno-unused-function -Wno-unused-variable
PSVNOPT = --no_state_map --no_abstraction --history_len=1

%:
	(cp puzzles/$*.psvn . && make $*.out && rm $*.psvn) || (echo -e "\n\e[1;31mError. \e[0mCompilation termined." && exit 1)
	echo -e "\n\e[1;36mCompilation successfully.\e[0m"

%.out: %.c src/*.cpp
	cd src; $(CXX) $(OPT2) *.cpp -include $< -o ../bin/$@ || (make clean && exit 1)
	make clean
	
%.c: %.psvn psvn/bin/psvn2c psvn2c_core.c psvn2c_state_map.c psvn2c_abstraction.c
	cd src; ../psvn/bin/psvn2c $(PSVNOPT) --name=$(*F) < ../puzzles/$< > $@
	
psvn/bin/psvn2c: psvn/src/psvn2c.cpp psvn/src/psvn2c.hpp psvn/src/psvn.cpp psvn/src/psvn.hpp
	$(CXX) $(OPT1) psvn/src/psvn2c.cpp psvn/src/psvn.cpp -o $@

psvn2c_core.c:
	cp psvn/src/psvn2c_core.c src/psvn2c_core.c

psvn2c_state_map.c:
	cp psvn/src/psvn2c_state_map.c src/psvn2c_state_map.c

psvn2c_abstraction.c:
	cp psvn/src/psvn2c_abstraction.c src/psvn2c_abstraction.c

.PRECIOUS: %.c

clean:
	rm -f src/*.c

