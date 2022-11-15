#include "coupling.hpp"

Coupling::Coupling(int dim, double beta) : grid_dim(dim), grid_size(dim * dim), chain_1(dim, -1, beta), chain_2(dim, 1, beta) {}

void Coupling::simulate() {
    long long int iterations;
    while (!converged()) {
        chain_1.flip();
        chain_2.flip();
        iterations++;
    }
    printf("Simulation of %d by %d grid converged after %lld iterations\n", grid_dim, grid_dim, iterations);
}

bool Coupling::converged() {
    for (int i = 0; i < grid_size; i++) {
        if (chain_1.grid[i] != chain_2.grid[i]) {
            return false;
        }
    }
    return true;
}