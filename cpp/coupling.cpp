#include "coupling.hpp"

Coupling::Coupling(): Coupling(0, 0) {}
Coupling::Coupling(int dim): Coupling(dim, 0) {}
Coupling::Coupling(int dim, double beta):
grid_dim(dim), grid_size(dim * dim), chain_1(dim, NEGATIVE, beta), chain_2(dim, POSITIVE, beta) {}

bool Coupling::simulate(unsigned int& iterations, unsigned int max) {
    iterations = 0;
    while (!converged() && iterations < max) {
        chain_1.flip();
        chain_2.flip();
        iterations++;
    }
    return converged();
}

bool Coupling::converged() {
    for (int i = 0; i < grid_size; i++) {
        if (chain_1.grid[i] != chain_2.grid[i]) {
            return false;
        }
    }
    return true;
}

void Coupling::print_spin() {
    chain_1.print_spin();
    chain_2.print_spin();
}