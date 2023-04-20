#include "simulation.hpp"

void* run_sim(void* arg) {
    thread_arg* ta = (thread_arg*)arg;
    auto start = std::chrono::steady_clock::now();
    int steps = ta->sim->run(ta->beta);
    std::chrono::duration<double, std::milli> diff = std::chrono::steady_clock::now() - start;
    ta->trials->push_back(steps);
    ta->times->push_back(diff.count());

    return nullptr;
}

void simulate(std::ostream& out, Simulation& sim, double beta_start, double beta_end, int betas, int runs) {
    int med_steps = -1;
    double med_time = -1;
    double beta = beta_start;
    std::vector<int> trials;
    std::vector<double> times;
    double beta_step = (beta_end - beta_start) / betas;

    thread_arg* tas = new thread_arg[runs];
    pthread_t* ptts = new pthread_t[runs];

    for (int bi = 0; bi < betas+1 && med_steps < sim.max_steps; bi++) {
        med_steps = -1;
        med_time = -1;
        trials.clear();
        times.clear();

        for (int i = 0; i < runs; i++) {
            tas[i].sim = &sim;
            tas[i].beta = beta;
            tas[i].trials = &trials;
            tas[i].times = &times;

            pthread_create(&ptts[i], nullptr, run_sim, (void *)&tas[i]);
        }

        for (int i = 0; i < runs; i++) {
            printf("\rbeta %f %d/%d %d steps %0.2f ms", beta, i, runs, med_steps, med_time);
            fflush(stdout);
            pthread_join(ptts[i], nullptr);
            sort(trials.begin(), trials.end());
            sort(times.begin(), times.end());
            med_steps = trials[trials.size() / 2];
            med_time = times[times.size() / 2];
        }

        printf("\rbeta %f mixed in %d steps %f ms\n", beta, med_steps, med_time);
        if (med_steps >= sim.max_steps) {
            printf("\rbeta %f failed to converge\n", beta);
        } else {
            out << beta << ", " << med_steps << ", " << std::fixed << med_time << "\n";
        }
        fflush(stdout);
        beta += beta_step;
    }

    delete[] tas;
    delete[] ptts;
}
