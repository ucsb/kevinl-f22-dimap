/*
https://en.wikipedia.org/wiki/Glauber_dynamics
*/

// #include <iostream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <random>
#include <cmath>

void print_grid(int grid[], int grid_dim) {
    for (int i = 0; i < (grid_dim * grid_dim); i++) {
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

void shuffle(int grid[], int grid_dim) {
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    for (int i = 0; i < (grid_dim * grid_dim); i++) {
        if (distribution(generator) < 0.5) {
            grid[i] = 1;
        } else {
            grid[i] = -1;
        }
    }
}

int choose_point(int grid_dim) {
    int return_me;
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, grid_dim * grid_dim);
    return_me = (int)distribution(generator);
    // printf("random from [0, %d): %d\n", grid_size, return_me);
    return return_me;
}

int neighbor_sum_p(int index, int grid[], int grid_dim) {
    int return_me = 0;
    if (index < 0 || index >= (grid_dim * grid_dim)) {
        printf("Attempted to sum neighbors for out-of-bounds index\n");
        exit(1);
    }
    if (index % grid_dim < (grid_dim - 1))
        return_me += (grid[index + 1] == 1);
    if (index % grid_dim > 0)
        return_me += (grid[index - 1] == 1);
    if (index / grid_dim < (grid_dim - 1))
        return_me += (grid[index + grid_dim] == 1);
    if (index / grid_dim > 0)
        return_me += (grid[index - grid_dim] == 1);
    return return_me;
}

int neighbor_sum_n(int index, int grid[], int grid_dim) {
    int return_me = 0;
    if (index < 0 || index >= (grid_dim * grid_dim)) {
        printf("Attempted to sum neighbors for out-of-bounds index\n");
        exit(1);
    }
    if (index % grid_dim < (grid_dim - 1))
        return_me += (grid[index + 1] == -1);
    if (index % grid_dim > 0)
        return_me += (grid[index - 1] == -1);
    if (index / grid_dim < (grid_dim - 1))
        return_me += (grid[index + grid_dim] == -1);
    if (index / grid_dim > 0)
        return_me += (grid[index - grid_dim] == -1);
    return return_me;
}

int neighbor_sum(int index, int grid[], int grid_dim) {
    int return_me = 0;
    if (index < 0 || index >= (grid_dim * grid_dim)) {
        printf("Attempted to sum neighbors for out-of-bounds index\n");
        exit(1);
    }
    if (index % grid_dim < (grid_dim - 1))
        return_me += grid[index + 1];
    if (index % grid_dim > 0)
        return_me += grid[index - 1];
    if (index / grid_dim < (grid_dim - 1))
        return_me += grid[index + grid_dim];
    if (index / grid_dim > 0)
        return_me += grid[index - grid_dim];
    return return_me;
}

int flip(int index, int grid[], int grid_dim, double& beta) {
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    
    double beta_p = 2.0 * beta * neighbor_sum_p(index, grid, grid_dim);
    double beta_n = 2.0 * beta * neighbor_sum_n(index, grid, grid_dim);
    double prob_p = std::exp(beta_p) / (std::exp(beta_n) + std::exp(beta_p));
    
    double rand = distribution(generator);
    printf("rand %f prob_p %f prob_n %f\n", rand, prob_p, 1 - prob_p);
    if (rand >= prob_p) {
        return 1;
    } else {
        return -1;
    }
}

int main(int argc, char** argv) {
    int grid_dim, grid_size, delta_e, iterations;
    int* grid;
    double beta;
    
    if (argc < 4) {
        printf("usage: executable grid-dim iterations beta\n");
        exit(1);
    }
    
    grid_dim = atoi(argv[1]);
    if (grid_dim <= 0) {
        printf("invalid grid dimension\n");
        exit(1);
    }
    grid_size = grid_dim * grid_dim;
    
    iterations = atoi(argv[2]);
    if (iterations < 0) {
        printf("invalid iteration argument\n");
        exit(1);
    }

    beta = std::stod(argv[3]);

    grid = new int[grid_size];

    printf("hello there!\n");
    
    shuffle(grid, grid_dim);
    print_grid(grid, grid_dim);


    int rand_index;
    for (int i = 0; i < iterations; i++) {
        // rand_index = choose_point(grid_dim);
        // printf("Sum at (%d,%d): %d\n", rand_index / grid_dim, rand_index % grid_dim, neighbor_sum(rand_index, grid, grid_dim));
        flip(choose_point(grid_dim), grid, grid_dim, beta);
    }

    delete[] grid;
    return 0;
}