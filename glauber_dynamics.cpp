/*
https://en.wikipedia.org/wiki/Glauber_dynamics
*/

// #include <iostream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <random>
#include <cmath>
#include <vector>

bool converged(int chain_1[], int chain_2[], int grid_dim) {
    for (int i = 0; i < (grid_dim * grid_dim); i++) {
        if (chain_1[i] != chain_2[i]) {
            return false;
        }
    }
    return true;
}

class MC {
public:
    std::vector<int> grid;

    MC() : MC(0, 1, 0) {}
    MC(int dim, double b) : MC(dim, 1, b) {}
    MC(int dim, int init_val, double b) : grid_dim(dim), grid_size(dim * dim), beta(b) {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
        generator = std::default_random_engine(seed);
        grid.resize(grid_size, init_val);
    }
    void print_plain() {
        for (int i = 0; i < grid_size; i++) {
            printf("%d ", grid[i]);
            if (i % grid_dim == grid_dim - 1) {
                printf("\n");
            }
        }
    }
    void print_spin() {
        for (int i = 0; i < grid_size; i++) {
            if (grid[i] == 1) {
                printf("+ ");
            } else {
                printf("- ");
            }
            if (i % grid_dim == grid_dim - 1) {
                printf("\n");
            }
        }
    }
    void flip() {
        flip(choose_point());
    }
    void flip(int index) {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        
        double beta_p = 2.0 * beta * neighbor_sum(index, 1);
        double beta_n = 2.0 * beta * neighbor_sum(index, -1);
        double prob_p = std::exp(beta_p) / (std::exp(beta_n) + std::exp(beta_p));
        
        double rand = distribution(generator);
        // printf("rand %f prob_p %f prob_n %f\n", rand, prob_p, 1 - prob_p);
        if (rand >= prob_p) {
            grid[index] = 1;
        } else {
            grid[index] = -1;
        }
    }
    void shuffle() {
        for (int i = 0; i < grid_size; i++) {
            flip(i);
        }
    }

private:
    int grid_dim;
    int grid_size;
    double beta;
    unsigned int seed;
    std::default_random_engine generator;

    int choose_point() {
        int return_me;
        // std::default_random_engine generator(seed);
        std::uniform_real_distribution<double> distribution(0.0, grid_size);
        return_me = (int)distribution(generator);
        // printf("random from [0, %d): %d\n", grid_size, return_me);
        return return_me;
    }

    int neighbor_sum(int index, int val) {
        int return_me = 0;
        if (index < 0 || index >= grid_size) {
            printf("Attempted to sum neighbors for out-of-bounds index\n");
            exit(1);
        }
        if (index % grid_dim < (grid_dim - 1))
            return_me += (grid[index + 1] == val);
        if (index % grid_dim > 0)
            return_me += (grid[index - 1] == val);
        if (index / grid_dim < (grid_dim - 1))
            return_me += (grid[index + grid_dim] == val);
        if (index / grid_dim > 0)
            return_me += (grid[index - grid_dim] == val);
        return return_me;
    }
};

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