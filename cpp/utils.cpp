#include "utils.hpp"

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_real_distribution<float> uniform(0.0,1.0);

// Avoids negative remainders
int mod(int a, int b) {
    return (b + (a % b)) % b;
}

unsigned char choose_color(unsigned char colors) {
    return (unsigned char)(uniform(generator) * colors);
}

bool keep_edge(double& beta) {
    return uniform(generator) > exp(-1 * beta);
}
