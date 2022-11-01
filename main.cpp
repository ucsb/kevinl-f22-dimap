// #include <iostream>
#include <cstdio>
#include <chrono>
#include <random>

int main(int argc, char** argv) {
    double sum;
    int rolls = 10000000;
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    printf("hello there!\n");
    
    for (int i = 0; i < rolls; i++) {
        sum += distribution(generator);
    }
    printf("uniform? %f\n", sum / rolls);
    
    return 0;
}