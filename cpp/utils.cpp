#include "utils.hpp"

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

// Avoids negative remainders
int mod(int a, int b) {
    return (b + (a % b)) % b;
}
