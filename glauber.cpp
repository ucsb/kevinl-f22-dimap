#include "glauber.hpp"

unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();;
std::default_random_engine generator = std::default_random_engine(seed);
std::uniform_real_distribution<double> normal(0.0, 1.0);

Glauber::Glauber() : Glauber(0, 1, 0) {}
Glauber::Glauber(int dim, double b) : Glauber(dim, 1, b) {}
Glauber::Glauber(int dim, bool init_spin, double b): beta(b), grid_dim(dim), grid_size(dim * dim) {
    if (grid_size > 0) {
        grid = new bool[grid_size];
        for (int i = 0; i < grid_size; i++) {
            grid[i] = init_spin;
        }
    }
}
Glauber::~Glauber() {
    delete[] grid;
}
Glauber& Glauber::operator=(const Glauber& other) {
    delete[] grid;
    grid = new bool[other.grid_size];
    for (int i = 0; i < other.grid_size; i++) {
        grid[i] = other.grid[i];
    }
    beta = other.beta;
    grid_dim = other.grid_dim;
    grid_size = other.grid_size;
    return *this;
}
void Glauber::print_plain() {
    for (int i = 0; i < grid_size; i++) {
        printf("%d ", grid[i]);
        if (i % grid_dim == grid_dim - 1) {
            printf("\n");
        }
    }
}
void Glauber::print_spin() {
    for (int i = 0; i < grid_size; i++) {
        if (grid[i] == POSITIVE) {
            printf("+ ");
        } else {
            printf("- ");
        }
        if (i % grid_dim == grid_dim - 1) {
            printf("\n");
        }
    }
}
void Glauber::flip() {
    flip(choose_point());
}
void Glauber::flip(int index) {
    int num_p = neighbor_sum(index, POSITIVE);
    double beta_p = 2.0 * beta * num_p;
    double beta_n = 2.0 * beta * (4 - num_p);
    double prob_p = std::exp(beta_p) / (std::exp(beta_n) + std::exp(beta_p));
    double rand = normal(generator);
    if (rand <= prob_p) {
        grid[index] = POSITIVE;
    } else {
        grid[index] = NEGATIVE;
    }
}
void Glauber::shuffle() {
    for (int i = 0; i < grid_size; i++) {
        flip(i);
    }
}
int Glauber::choose_point() {
    return (int)(normal(generator) * grid_size);;
}

int Glauber::neighbor_sum(int index, int val) {
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