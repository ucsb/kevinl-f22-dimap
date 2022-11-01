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

int get_val(int index, char grid[], int& grid_dim) {
    if (index < 0 || index >= (grid_dim * grid_dim)) {
        printf("Attempted to access value at out-of-bounds index\n");
        exit(1);
    } else if (grid[index] == '+') {
        return 1;
    } else {
        return -1;
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

int neighbor_sum(int index, char grid[], int grid_dim) {
    int return_me = 0;
    if (index < 0 || index >= (grid_dim * grid_dim)) {
        printf("Attempted to sum neighbors for out-of-bounds index\n");
        exit(1);
    }
    if (index % grid_dim < (grid_dim - 1))
        return_me += get_val(index + 1, grid, grid_dim);
    if (index % grid_dim > 0)
        return_me += get_val(index - 1, grid, grid_dim);
    if (index / grid_dim < (grid_dim - 1))
        return_me += get_val(index + grid_dim, grid, grid_dim);
    if (index / grid_dim > 0)
        return_me += get_val(index - grid_dim, grid, grid_dim);
    printf("Sum at (%d,%d): %d\n", index / grid_dim, index % grid_dim, return_me);
    return return_me;
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
    grid = new char[grid_size];

    printf("hello there!\n");
    
    shuffle(grid, grid_dim);
    print_grid(grid, grid_dim);

    for (int i = -1; i < grid_size + 1; i++) {
        neighbor_sum(i, grid, grid_dim);
    }
    
    delete[] grid;
    return 0;
}