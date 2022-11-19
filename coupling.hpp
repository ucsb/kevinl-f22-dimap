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

private:
    int grid_dim;
    int grid_size;
    Glauber chain_1;
    Glauber chain_2;
    pthread_t ptid_1, ptid_2;
    bool converged();
};

#endif