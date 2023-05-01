#include <random>
#include <chrono>

extern unsigned seed;
extern std::default_random_engine generator;
extern std::uniform_real_distribution<float> uniform;

// Avoids negative remainders
int mod(int a, int b);

unsigned char choose_color(unsigned char colors);

bool keep_edge(double& beta);
