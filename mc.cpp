#include "mc.hpp"

MC::MC() : MC(0, 1, 0) {}
MC::MC(int dim, double b) : MC(dim, 1, b) {}
MC::MC(int dim, int init_val, double b) : grid_dim(dim), grid_size(dim * dim), beta(b) {
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator = std::default_random_engine(seed);
    grid.resize(grid_size, init_val);
}
void MC::print_plain() {
    for (int i = 0; i < grid_size; i++) {
        printf("%d ", grid[i]);
        if (i % grid_dim == grid_dim - 1) {
            printf("\n");
        }
    }
}
void MC::print_spin() {
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
void MC::flip() {
    flip(choose_point());
}
void MC::flip(int index) {
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
void MC::shuffle() {
    for (int i = 0; i < grid_size; i++) {
        flip(i);
    }
}
int MC::choose_point() {
    int return_me;
    // std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, grid_size);
    return_me = (int)distribution(generator);
    // printf("random from [0, %d): %d\n", grid_size, return_me);
    return return_me;
}

int MC::neighbor_sum(int index, int val) {
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