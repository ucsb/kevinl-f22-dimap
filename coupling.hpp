#ifndef COUPLING_H
#define COUPLING_H

#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include "glauber.hpp"

class Coupling {
public:
    Coupling(int dim, double beta);
    void simulate();
    void simulate_threaded();
    void benchmark();

private:
    int grid_dim;
    int grid_size;
    double beta;
    Glauber chain_1;
    Glauber chain_2;
    long long int iterations;
    pthread_t ptid_1, ptid_2;
    bool converged();
};

#endif