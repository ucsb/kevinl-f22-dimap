#include "simulation.hpp"

void simulate(std::ostream& out, Simulation& sim, double beta_start, double beta_end, double beta_step, int runs) {
    int med_steps = -1, steps;
    double beta = beta_start;
    double med_time;
    std::vector<int> trials;
    std::vector<double> times;

    while (med_steps < sim.max_steps && beta <= beta_end) {
        trials.clear();
        times.clear();
        std::chrono::duration<double, std::milli> diff;
        for (int i = 0; i < runs; i++) {
            printf("\rrun %d/%d", i+1, runs);
            fflush(stdout);
            auto start = std::chrono::steady_clock::now();
            steps = sim.run(beta);
            diff = std::chrono::steady_clock::now() - start;
            trials.push_back(steps);
            times.push_back(diff.count());
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
        sort(trials.begin(), trials.end());
        sort(times.begin(), times.end());
        med_steps = trials[trials.size() / 2];
        med_time = times[times.size() / 2];
        if (med_steps < sim.max_steps) {
            printf("beta %f mixed in %d steps %f ms\n", beta, med_steps, med_time);
            out << beta << ", " << med_steps << ", " << std::fixed << med_time << "\n";
        } else {
            printf("beta %f failed to converge\n", beta);
        }
        fflush(stdout);
        beta += beta_step;
    };
}
