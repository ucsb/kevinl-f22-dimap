#ifndef MC_H
#define MC_H

#include <chrono>
#include <random>
#include <cmath>
#include <vector>

class MC {
public:
    std::vector<int> grid;
    MC();
    MC(int dim, double b);
    MC(int dim, int init_val, double b);
    void print_plain();
    void print_spin();
    void flip();
    void flip(int index);
    void shuffle();

private:
    int grid_dim;
    int grid_size;
    double beta;
    unsigned int seed;
    std::default_random_engine generator;
    int choose_point();
    int neighbor_sum(int index, int val);
};

#endif