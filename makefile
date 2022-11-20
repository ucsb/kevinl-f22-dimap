EXECS: ising benchmark

all: $(EXECS)

glauber.o: glauber.hpp glauber.cpp
	g++ -Wall -pedantic -c -o glauber.o glauber.cpp

coupling.o: coupling.hpp coupling.cpp
	g++ -Wall -pedantic -c -o coupling.o coupling.cpp -lpthread

ising: ising.cpp coupling.hpp coupling.o glauber.hpp glauber.o
	g++ -Wall -pedantic -o ising ising.cpp coupling.o glauber.o -lpthread

benchmark: benchmark.cpp coupling.hpp coupling.o glauber.hpp glauber.o
	g++ -Wall -pedantic -o benchmark benchmark.cpp coupling.o glauber.o -lpthread

clean:
	/bin/rm -f *.o core ising benchmark