#include <fstream>
#include <string>
#include "swendsen_wang.hpp"
#include "metropolis_glauber.hpp"
#include "heat_bath_glauber.hpp"
// #include "mcmcmc.hpp"

using namespace std;

ofstream file;

struct thread_arg {
    Chain* chain;
    float beta;
    std::vector<int>* trials;
    std::vector<float>* times;
};

void* run_sim(void* arg) {
    thread_arg* ta = (thread_arg*)arg;
    auto start = std::chrono::steady_clock::now();
    int steps = ta->chain->run(ta->beta);
    std::chrono::duration<float, std::milli> diff = std::chrono::steady_clock::now() - start;
    ta->trials->push_back(steps);
    ta->times->push_back(diff.count());

    return nullptr;
}

void* user_input(void* arg) {
    string input;
    while (true) {
        getline(cin, input);
        if (input.size() > 0) {
            file.close();
            exit(0);
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 7)
    {
        printf("usage: simulate.cpp dim colors beta_start beta_end betas runs\n");
        exit(1);
    }

    int max_time = 120000;
    int med_steps = -1;
    float med_time = -1;
    int dim = stoi(argv[1]);
    color_t colors = stoi(argv[2]);

    if (colors < 2)
    {
        printf("At least 2 colors required\n");
        exit(1);
    }

    float beta_start = stod(argv[3]);
    float beta_end = stod(argv[4]);
    int betas = stoi(argv[5]);
    int runs = stoi(argv[6]);
    vector<int> trials;
    vector<float> times;
    float beta_step = (beta_end - beta_start) / betas;

    thread_arg* tas = new thread_arg[runs];
    pthread_t* ptts = new pthread_t[runs];

    float beta = beta_start;

    file.open("simulate.csv");

    pthread_t ptt;
    pthread_create(&ptt, nullptr, user_input, nullptr);

    for (int bi = 0; bi < betas+1 && med_time < max_time; bi++)
    {
        med_steps = -1;
        med_time = -1;
        trials.clear();
        times.clear();

        for (int i = 0; i < runs; i++)
        {
            // tas[i].chain = new Swendsen_Wang_Grid(dim, colors);
            tas[i].chain = new Swendsen_Wang_Complete(dim, colors);
            // tas[i].chain = new Wolff_Complete(dim, colors);
            // tas[i].chain = new MCMCMC_Grid(dim, 2, dim);
            // tas[i].chain = new Metropolis_Glauber_Grid(dim, colors);
            // tas[i].chain = new Metropolis_CFTP_Grid(dim);
            // tas[i].chain = new Metropolis_Glauber_Complete(dim, colors);
            // tas[i].chain = new Metropolis_CFTP_Complete(dim);
            // tas[i].chain = new Heat_Bath_Glauber_Grid(dim, colors);
            // tas[i].chain = new Heat_Bath_Glauber_Grid(dim);
            // tas[i].chain = new Heat_Bath_CFTP_Grid(dim);
            // tas[i].chain = new Heat_Bath_Glauber_Complete(dim, colors);
            // tas[i].chain = new Heat_Bath_CFTP_Complete(dim);

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

        for (int i = 0; i < runs; i++) {
            delete tas[i].chain;
        }

        printf("\rbeta %f mixed in %d steps %f ms\n", beta, med_steps, med_time);
        file << beta << ", " << med_steps << ", " << std::fixed << med_time << "\n";
        if (med_time >= max_time) {
            printf("\rbeta %f took too long to converge\n", beta);
        }
        fflush(stdout);
        beta += beta_step;
    }
    delete[] tas;
    delete[] ptts;

    file.close();
    pthread_cancel(ptt);
    pthread_join(ptt, nullptr);
}
