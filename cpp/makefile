CFLAGS = -Wall -pedantic -std=c++20
EXECS = potts-mixing

all: $(EXECS)

grid.o: grid.hpp grid.cpp
	g++ $(CFLAGS) -c -o grid.o grid.cpp

utils.o: utils.hpp utils.cpp grid.hpp
	g++ $(CFLAGS) -c -o utils.o utils.cpp

simulation.o: simulation.hpp simulation.cpp
	g++ $(CFLAGS) -c -o simulation.o simulation.cpp

potts-mixing: potts-mixing.cpp simulation.o grid.o utils.o
	g++ $(CFLAGS) -o potts-mixing potts-mixing.cpp simulation.o grid.o utils.o

clean:
	/bin/rm -f *.o core $(EXECS)
