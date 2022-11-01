// #include <iostream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <random>

void print_grid(char grid[], int grid_dim) {
    for (int i = 0; i < (grid_dim * grid_dim); i++) {
        printf("%c ", grid[i]);
        if (i % grid_dim == grid_dim - 1) {
            printf("\n");
        }
    }
}

void shuffle(char grid[], int grid_dim) {
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    for (int i = 0; i < (grid_dim * grid_dim); i++) {
        if (distribution(generator) < 0.5) {
            grid[i] = '+';
        } else {
            grid[i] = '-';
        }
    }
}

int choose_point(int grid_dim) {
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, grid_dim * grid_dim);
    return (int)distribution(generator);
}

int main(int argc, char** argv) {
    int grid_dim, grid_size;
    char* grid;
    
    if (argc < 2) {
        printf("usage: executable grid-dim\n");
        exit(1);
    }
    
    grid_dim = atoi(argv[1]);
    grid_size = grid_dim * grid_dim;
    grid = new char[grid_dim];

    printf("hello there!\n");
    
    shuffle(grid, grid_dim);
    print_grid(grid, grid_dim);

    printf("random from [0, %d): %d\n", grid_size, choose_point(3));
    
    return 0;
}