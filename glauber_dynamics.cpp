/*
https://en.wikipedia.org/wiki/Glauber_dynamics
*/

// #include <iostream>
#include "coupling.hpp"

int main(int argc, char** argv) {
    int grid_dim;
    double beta;
    
    if (argc < 3) {
        printf("usage: executable grid-dim beta\n");
        exit(1);
    }
    
    grid_dim = atoi(argv[1]);
    if (grid_dim <= 0) {
        printf("invalid grid dimension\n");
        exit(1);
    }

    beta = std::stod(argv[2]);
    
    Coupling c(grid_dim, beta);
    c.simulate();

    return 0;
}