// #include <iostream>
#include <cstdio>
#include <chrono>
#include <random>

void print_grid(int grid[], int grid_dim) {
    for (int i = 0; i < (grid_dim * grid_dim); i++) {
        printf("%d ", grid[i]);
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
            grid[i] = 0;
        }
    }
}

int main(int argc, char** argv) {
    int grid[9];
    
    printf("hello there!\n");
    
    shuffle(grid, 3);
    print_grid(grid, 3);
    
    return 0;
}