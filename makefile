EXECS: ising

all: $(EXECS)

mc.o: mc.hpp mc.cpp
	g++ -Wall -pedantic -c -o mc.o mc.cpp

coupling.o: coupling.hpp coupling.cpp
	g++ -Wall -pedantic -c -o coupling.o coupling.cpp

ising: glauber_dynamics.cpp coupling.hpp coupling.o mc.hpp mc.o
	g++ -Wall -pedantic -o ising glauber_dynamics.cpp coupling.o mc.o

clean:
	/bin/rm -f *.o core ising