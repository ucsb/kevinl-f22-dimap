#ifndef COUPLING_H
#define COUPLING_H

#include <cstdio>
#include <cstdlib>
#include "mc.hpp"

class Coupling {
public:
    Coupling(int dim, double beta);
    void simulate();

private:
    int grid_dim;
    int grid_size;
    MC chain_1;
    MC chain_2;
    bool converged();
};

#endif