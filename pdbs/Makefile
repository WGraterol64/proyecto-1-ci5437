ROOT = ../psvn
PUZZLES = ../puzzles
PSVNOPT = --state_map --backward_moves --fwd_history_len=0 --bwd_history_len=0
OPT = -Wall -O3 -Wno-unused-function -Wno-unused-variable
CXX = g++

%:  
	make abstractor.out
	./abstractor.out $(PUZZLES)/$*.psvn $* < ./src/abstraction > /dev/null
	make $*.c
	$(CXX) $(OPT) $(ROOT)/global/dist.cpp -include $*.c -o ./$*.dist
	./$*.dist > $*.pdb
	$(CXX) $(OPT) ./src/make_state_map.cpp --include $*.c -o ./make_state_map_$*.out 
	./make_state_map_$*.out $*.pdb $*.state_map
	make clean

%.out:
	$(CXX) $(OPT) $(ROOT)/src/abstractor.cpp $(ROOT)/src/psvn.cpp -o $*.out

psvn2c_core.c:
	cp $(ROOT)/src/psvn2c_core.c ./psvn2c_core.c

psvn2c_state_map.c:
	cp $(ROOT)/src/psvn2c_state_map.c ./psvn2c_state_map.c

psvn2c_abstraction.c:
	cp $(ROOT)/src/psvn2c_abstraction.c ./psvn2c_abstraction.c

%.c: psvn2c_core.c psvn2c_state_map.c psvn2c_abstraction.c
	$(ROOT)/bin/psvn2c $(PSVNOPT) --name=$* < $*.psvn > $@



clean:
	rm -f *.c *.abst *.psvn *.out *.pdb *.dist


