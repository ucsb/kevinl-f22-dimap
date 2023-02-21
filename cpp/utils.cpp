#include <random>
#include <chrono>

#include "grid.hpp"

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_real_distribution<double> uniform(0.0,1.0);

// Avoids negative remainders
int mod(int a, int b) {
    return (b + (a % b)) % b;
}

int choose_point(const Grid& c) {
    return int(uniform(generator) * c.size);
}

int choose_color(int colors) {
    return (int)(uniform(generator) * colors);
}

void sum_neighbors(const Grid& g, int& index, int counts[]) {
    counts[g.graph[mod(index+g.w, g.size)]]++;
    counts[g.graph[mod(index-g.w, g.size)]]++;
    counts[g.graph[mod(index-1, g.size)]]++;
    counts[g.graph[mod(index+1, g.size)]]++;
}

bool keep_edge(double& beta) {
    return uniform(generator) > exp(-1 * beta);
}
