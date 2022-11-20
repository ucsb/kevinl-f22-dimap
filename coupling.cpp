#include "coupling.hpp"
#include <ctime>

Coupling::Coupling(int dim, double b):
grid_dim(dim), grid_size(dim * dim), beta(b), chain_1(dim, NEGATIVE, beta), chain_2(dim, POSITIVE, beta) {}

void Coupling::simulate() {
    iterations = 0;
    while (!converged()) {
        chain_1.flip();
        chain_2.flip();
        iterations++;
    }
    // printf("Simulation of %d by %d grid converged after %lld iterations\n", grid_dim, grid_dim, iterations);
}

void Coupling::simulate_threaded() {
    iterations = 0;
    while (!converged()) {
        pthread_create(&ptid_1, NULL, Glauber::pt_flip, (void*)&chain_1);
        pthread_create(&ptid_2, NULL, Glauber::pt_flip, (void*)&chain_2);
        pthread_join(ptid_1, NULL);
        pthread_join(ptid_2, NULL);
        iterations++;
    }
    // printf("Simulation of %d by %d grid converged after %lld iterations\n", grid_dim, grid_dim, iterations);
}

bool Coupling::converged() {
    for (int i = 0; i < grid_size; i++) {
        if (chain_1.grid[i] != chain_2.grid[i]) {
            return false;
        }
    }
    return true;
}

void Coupling::benchmark() {
    double ratio = 0;
    int runs = 100;
    for (int i = 0; i < runs; i++) {
        clock_t start = clock();
        simulate();
        ratio += ((clock() - start) / iterations);
        chain_1 = Glauber(grid_dim, NEGATIVE, beta);
        chain_2 = Glauber(grid_dim, POSITIVE, beta);
    }
    printf("Average time per run %f\n", ratio / runs);
    ratio = 0;
    for (int i = 0; i < runs; i++) {
        clock_t start = clock();
        simulate_threaded();
        ratio += ((clock() - start) / iterations);
        chain_1 = Glauber(grid_dim, NEGATIVE, beta);
        chain_2 = Glauber(grid_dim, POSITIVE, beta);
    }
    printf("Average time per run threaded %f\n", ratio / runs);
}