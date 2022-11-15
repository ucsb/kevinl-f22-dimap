EXECS: ising

all: $(EXECS)

mc.o: mc.hpp mc.cpp
	g++ -Wall -pedantic -c -o mc.o mc.cpp

ising: glauber_dynamics.cpp mc.hpp mc.o
	g++ -Wall -pedantic -o ising glauber_dynamics.cpp mc.o

clean:
	/bin/rm -f *.o core ising