#include <random>
#include <chrono>
#include "grid.hpp"

extern unsigned seed;
extern std::default_random_engine generator;
extern std::uniform_real_distribution<double> uniform;

// Avoids negative remainders
int mod(int a, int b);

int choose_point(const Grid& c);

int choose_color(int colors);

void sum_neighbors(const Grid& g, int& index, int counts[]);

bool keep_edge(double& beta);
