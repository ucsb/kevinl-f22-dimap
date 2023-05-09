#include "utils.hpp"

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_real_distribution<float> uniform(0.0,1.0);

// Avoids negative remainders
int mod(int a, int b) {
    return (b + (a % b)) % b;
}
