#ifndef COUPLING_H
#define COUPLING_H

#include <cstdio>
#include <cstdlib>
#include <climits>
#include "glauber.hpp"

class Coupling {
public:
    Coupling();
    Coupling(int dim);
    Coupling(int dim, double beta);
    bool simulate(unsigned int& iterations, unsigned int max);
    void print_spin();

private:
    int grid_dim;
    int grid_size;
    Glauber chain_1;
    Glauber chain_2;
    bool converged();
};

#endif