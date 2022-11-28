EXECS: ising

all: $(EXECS)

glauber.o: glauber.hpp glauber.cpp
	g++ -Wall -pedantic -c -o glauber.o glauber.cpp

coupling.o: coupling.hpp coupling.cpp
	g++ -Wall -pedantic -c -o coupling.o coupling.cpp

ising: ising.cpp coupling.hpp coupling.o glauber.hpp glauber.o
	g++ -Wall -pedantic -o ising ising.cpp coupling.o glauber.o

report: report.cpp coupling.hpp coupling.o glauber.hpp glauber.o
	g++ -Wall -pedantic -std=c++20 -o report report.cpp coupling.o glauber.o

clean:
	/bin/rm -f *.o core ising