CFLAGS = -Wall -pedantic -std=c++20 -pthread
EXECS = simulate
OBJS = grid.o heat_bath_glauber.o metropolis_glauber.o swendsen_wang.o utils.o

# Object files

grid.o: grid.cpp grid.hpp
	g++ $(CFLAGS) -c -o $@ grid.cpp

heat_bath_glauber.o: heat_bath_glauber.cpp heat_bath_glauber.hpp chain.hpp
	g++ $(CFLAGS) -c -o $@ heat_bath_glauber.cpp

metropolis_glauber.o: metropolis_glauber.cpp metropolis_glauber.hpp chain.hpp
	g++ $(CFLAGS) -c -o $@ metropolis_glauber.cpp

swendsen_wang.o: swendsen_wang.cpp swendsen_wang.hpp chain.hpp staticq.hpp
	g++ $(CFLAGS) -c -o $@ swendsen_wang.cpp

utils.o: utils.cpp utils.hpp
	g++ $(CFLAGS) -c -o $@ utils.cpp

# Executables

all: $(EXECS)

benchmark: benchmark.cpp $(OBJS)
	g++ $(CFLAGS) -o $@ $^

counts: counts.cpp $(OBJS)
	g++ $(CFLAGS) -o $@ $^

debug: debug.cpp $(OBJS)
	g++ $(CFLAGS) -o $@ $^

simulate: simulate.cpp $(OBJS)
	g++ $(CFLAGS) -o $@ $^

# Clean up

clean:
	/bin/rm -f *.o core $(EXECS)
