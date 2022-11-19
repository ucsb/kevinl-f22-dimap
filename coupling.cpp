#include "coupling.hpp"

Coupling::Coupling(int dim, double beta):
grid_dim(dim), grid_size(dim * dim), chain_1(dim, NEGATIVE, beta), chain_2(dim, POSITIVE, beta) {}

void Coupling::simulate() {
    long long int iterations = 0;
    while (!converged()) {
        pthread_create(&ptid_1, NULL, Glauber::pt_flip, (void*)&chain_1);
        pthread_create(&ptid_2, NULL, Glauber::pt_flip, (void*)&chain_2);
        pthread_join(ptid_1, NULL);
        pthread_join(ptid_2, NULL);
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