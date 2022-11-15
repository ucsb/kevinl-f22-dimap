/*
https://en.wikipedia.org/wiki/Glauber_dynamics
*/

// #include <iostream>
#include <cstdio>
#include <cstdlib>
#include "mc.hpp"

bool converged(int chain_1[], int chain_2[], int grid_dim) {
    for (int i = 0; i < (grid_dim * grid_dim); i++) {
        if (chain_1[i] != chain_2[i]) {
            return false;
        }
    }
    return true;
}

class Coupling {
public:
    Coupling(int dim, double beta) : grid_dim(dim), grid_size(dim * dim), chain_1(dim, -1, beta), chain_2(dim, 1, beta) {}
    void simulate() {
        long long int iterations;
        while (!converged()) {
            chain_1.flip();
            chain_2.flip();
            iterations++;
        }
        printf("Simulation of %d by %d grid converged after %lld iterations\n", grid_dim, grid_dim, iterations);
    }

private:
    int grid_dim;
    int grid_size;
    MC chain_1;
    MC chain_2;

    bool converged() {
        for (int i = 0; i < grid_size; i++) {
            if (chain_1.grid[i] != chain_2.grid[i]) {
                return false;
            }
        }
        return true;
    }
};

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