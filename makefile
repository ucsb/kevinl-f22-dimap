EXECS: ising

all: $(EXECS)

ising: glauber_dynamics.cpp
	g++ -Wall -o ising glauber_dynamics.cpp

clean:
	/bin/rm -f *.o core ising