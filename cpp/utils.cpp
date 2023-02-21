#include <random>
#include <chrono>

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_real_distribution<double> uniform(0.0,1.0);

// Avoids negative remainders
int mod(int a, int b) {
    return (b + (a % b)) % b;
}

int choose_color(int colors) {
    return (int)(uniform(generator) * colors);
}

bool keep_edge(double& beta) {
    return uniform(generator) > exp(-1 * beta);
}
