#include "glauber.hpp"

Glauber::Glauber() : Glauber(0, 1, 0) {}
Glauber::Glauber(int dim, double b) : Glauber(dim, 1, b) {}
Glauber::Glauber(int dim, bool init_spin, double b): grid_dim(dim), grid_size(dim * dim), beta(b) {
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator = std::default_random_engine(seed);
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
    if (grid_dim != other.grid_dim) {
        printf("Bad assignment: cannot assign grids of different dimensions\n");
        exit(1);
    }
    delete[] grid;
    grid = new bool[grid_size];
    memcpy(grid, other.grid, grid_size);
    beta = other.beta;
    seed = other.seed;
    generator = other.generator;
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
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    double beta_p = 2.0 * beta * neighbor_sum(index, POSITIVE);
    double beta_n = 2.0 * beta * neighbor_sum(index, NEGATIVE);
    double prob_p = std::exp(beta_p) / (std::exp(beta_n) + std::exp(beta_p));

    double rand = distribution(generator);
    // printf("rand %f prob_p %f prob_n %f\n", rand, prob_p, 1 - prob_p);
    if (rand >= prob_p) {
        grid[index] = POSITIVE;
    } else {
        grid[index] = NEGATIVE;
    }
}
void* Glauber::pt_flip(void* pt_arg) {
    ((Glauber*)pt_arg)->flip();
    printf("Glauber %p done\n", pt_arg);
    return NULL;
}
void Glauber::shuffle() {
    for (int i = 0; i < grid_size; i++) {
        flip(i);
    }
}
int Glauber::choose_point() {
    int return_me;
    std::uniform_real_distribution<double> distribution(0.0, grid_size);
    return_me = (int)distribution(generator);
    // printf("random from [0, %d): %d\n", grid_size, return_me);
    return return_me;
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