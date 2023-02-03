#include <numeric>
#include <fstream>
#include "coupling.hpp"

int main(int argc, char** argv) {
    int grid_dim;
    unsigned int max_iterations;
    double beta_end, beta_step;
    if (argc < 5) {
        printf("usage: executable grid-dim beta-end beta-step max-iterations\n");
        exit(1);
    }
    grid_dim = atoi(argv[1]);
    if (grid_dim <= 0) {
        printf("invalid grid dimension\n");
        exit(1);
    }
    beta_end = std::stod(argv[2]);
    beta_step = std::stod(argv[3]);
    max_iterations = std::stoi(argv[4]);

    unsigned int iterations;
    bool converged;
    unsigned int average_it = 0;
    Coupling c;
    
    std::ofstream csv_out("ising.csv");

    for (double beta = 0.0; beta < beta_end; beta += beta_step) {
        printf("beta %f\n", beta);
        for (int i = 0; i < 10; i++) {
            c = Coupling(grid_dim, 1.0 / beta);
            converged = c.simulate(iterations, max_iterations);
            if (converged) {
                average_it = std::midpoint(average_it, iterations);
                // printf("\tconverged 1 | iterations %d | avg iterations %d\n", iterations, average_it);
            } else {
                // printf("\tconverged 0 | iterations na | avg iterations na\n");
                csv_out.close();
                exit(0);
            }
        }
        csv_out << beta << "," << average_it << "\n";
    }
    csv_out.close();
    return 0;
}