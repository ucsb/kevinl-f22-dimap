#ifndef GLAUBER_H
#define GLAUBER_H

#include <chrono>
#include <random>
#include <cmath>
#include <cstring>

#define POSITIVE 1
#define NEGATIVE 0

class Glauber {
public:
    bool* grid = NULL;
    double beta = 0.0f;
    Glauber();
    Glauber(int dim, double b);
    Glauber(int dim, bool init_spin, double b);
    ~Glauber();
    Glauber& operator=(const Glauber& other);
    void print_plain();
    void print_spin();
    void flip();
    void flip(int index);
    void shuffle();

private:
    int grid_dim;
    int grid_size;
    int choose_point();
    int neighbor_sum(int index, int val);
};

#endif