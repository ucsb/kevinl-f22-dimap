#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <chrono>

extern unsigned seed;

// Avoids negative remainders
int mod(int a, int b);

unsigned char choose_color(unsigned char colors);

bool keep_edge(double& beta);

#endif
