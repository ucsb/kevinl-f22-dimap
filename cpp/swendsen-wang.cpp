#include <cstdio>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>

#include "grid.hpp"

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);
uniform_real_distribution<double> uniform(0.0,1.0);

int choose_point(const Grid& c) {
    return int(uniform(generator) * c.size);
}

// Avoids negative remainders
int mod(int a, int b) {
    return (b + (a % b)) % b;
}

void sum_neighbors(const Grid& g, int& index, int counts[]) {
    counts[g.graph[mod(index+g.w, g.size)]]++;
    counts[g.graph[mod(index-g.w, g.size)]]++;
    counts[g.graph[mod(index-1, g.size)]]++;
    counts[g.graph[mod(index+1, g.size)]]++;
}

void glauber_ising_flip(Grid& g, int& index, double& beta, double& rand) {
    int counts[2] {0};
    sum_neighbors(g, index, counts);
    double beta_n = beta * counts[0];
    double beta_p = beta * counts[1];
    double prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p));
    g.set(index, (int)(rand <= prob_p));
}

void glauber_potts_flip(Grid& g, int index, double& beta, double rand) {
    int* counts = new int[g.colors] {0};
    double* exps = new double[g.colors] {0};
    double sum = 0.0;
    sum_neighbors(g, index, counts);
    for (int c = 0; c < g.colors; c++) {
        sum += exp(beta * counts[c]);
        exps[c] = sum;
    }
    sum = 1.0 / sum;
    for (int c = 0; c < g.colors; c++) {
        if (rand < (exps[c] * sum)) {
            g.set(index, c);
            break;
        }
    }
    delete[] counts;
    delete[] exps;
}

int choose_color(int colors) {
    return (int)(uniform(generator) * colors);
}

bool keep_edge(double& beta) {
    return uniform(generator) > exp(-1 * beta);
}

void swendsen_wang_flip(Grid& g, double& beta) {
    // bool** visited[g.h];
    // for (int i = 0; i < g.h; i++)
    //     visited[i] = (bool[g.w] {false});
    bool* visited = new bool[g.size] {false};

    int q_head = 0, q_tail = 0, index, old_color, new_color;
    int top, bottom, left, right;
    // pair<int,int> queue[g.size];
    int* queue = new int[g.size];
    for (int i = 0; i < g.size; i++) {
        if (visited[i] == false) {
            visited[i] = true;
            queue[q_tail] = i;
            q_tail++;
            old_color = g.graph[i];
            new_color = choose_color(g.colors);
            while (q_head != q_tail) {
                index = queue[q_head];
                q_head++;
                g.set(index, new_color);
                top = mod(index-g.w, g.size);
                bottom = mod(index+g.w, g.size);
                left = mod(index-1, g.size);
                right = mod(index+1, g.size);
                if (visited[top] == false && g.graph[top] == old_color && keep_edge(beta)) {
                    visited[top] = true;
                    queue[q_tail] = top;
                    q_tail++;
                }
                if (visited[bottom] == false && g.graph[bottom] == old_color && keep_edge(beta)) {
                    visited[bottom] = true;
                    queue[q_tail] = bottom;
                    q_tail++;
                }
                if (visited[left] == false && g.graph[left] == old_color && keep_edge(beta)) {
                    visited[left] = true;
                    queue[q_tail] = left;
                    q_tail++;
                }
                if (visited[right] == false && g.graph[right] == old_color && keep_edge(beta)) {
                    visited[right] = true;
                    queue[q_tail] = right;
                    q_tail++;
                }
            }
        }
    }
    delete[] visited;
    delete[] queue;
}

int glauber_ising_coupling(int dim, double beta, int max_steps) {
    int index, steps = 0;
    int* counts;
    double rand;
    Grid zeros(dim, 2);
    Grid ones(dim, 2);
    zeros.set_all(0);
    ones.set_all(1);
    while (steps < max_steps && ones != zeros) {
        index = choose_point(zeros);
        rand = uniform(generator);
        glauber_ising_flip(zeros, index, beta, rand);
        glauber_ising_flip(ones, index, beta, rand);
        steps++;
    }
    return steps;
}

bool grids_same(Grid grids[], int size) {
    for (int i = 1; i < size; i++)
        if (grids[0] != grids[i]) return false;
    return true;
}

int glauber_potts_coupling(int dim, double beta, int max_steps, int colors) {
    int index, steps = 0;
    int* counts;
    double rand;
    Grid* grids = new Grid[colors];
    for (int c = 0; c < colors; c++) {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }
    while (steps < max_steps && !grids_same(grids, colors)) {
        index = choose_point(grids[0]);
        rand = uniform(generator);
        for (int c = 0; c < colors; c++)
            glauber_potts_flip(grids[c], index, beta, rand);
        steps++;
    }
    delete[] grids;
    return steps;
}

bool mag_converged(const Grid& grid, int& target) {
    for (int c = 0; c < grid.colors; c++) {
        if (abs(grid.counts[c] - target) > grid.colors)
            return false;
    }
    return true;
}

int glauber_potts_magnetization(int dim, double beta, int max_steps, int colors) {
    int steps = 0, target;
    Grid g(dim, colors);
    g.set_all(0);
    target = (int)(1.0 / colors * g.size);
    while (steps < max_steps && !mag_converged(g, target)) {
        glauber_potts_flip(g, choose_point(g), beta, uniform(generator));
        steps++;
    }
    return steps;
}

bool mag_match(const Grid grids[], int num) {
    for (int i = 0; i < num; i++) {
        for (int j = i + 1; j < num; j++) {
            for (int c = 0; c < grids[i].colors; c++) {
                if (abs(grids[i].counts[c] - grids[j].counts[c]) > grids[i].colors)
                    return false;
            }
        }
    }
    return true;
}

int swendsen_ising_magnetization(int dim, double beta, int max_steps) {
    int steps = 0;
    Grid grids[] = {Grid(dim, 2), Grid(dim, 2), Grid(dim, 2)};
    grids[0].set_all(0);
    grids[1].set_all(1);
    grids[2].chessboard();
    while (steps < max_steps && !mag_match(grids, 3)) {
        for (int i = 0; i < 3; i++)
            swendsen_wang_flip(grids[i], beta);
        steps += 1;
    }
    return steps;
}

int main(int argc, char** argv) {
    int colors = 3;
    int dim = 100;
    int max_steps = 50000000;
    int med_steps;
    double med_time;
    double step_size = .01;
    double beta = 0;
    vector<int> trials(10, 0);
    vector<double> times;
    ofstream file;

    setvbuf(stdout, NULL, _IOLBF, 0);

    if (argc != 3) {
        printf("Usage: swendsen-wang.cpp dim output-file\n");
        exit(1);
    }

    dim = stoi(argv[1]);

    file.open(string(argv[2]) + ".csv");

    double b_crit = log(1 + sqrt(colors)) / 2;
    printf("%f is the critical beta\n", b_crit);

    while (med_steps < max_steps && beta < b_crit * 2) {
        chrono::duration<double, milli> diff;
        times.clear();
        for (int i = 0; i < trials.size(); i++) {
            auto start = chrono::steady_clock::now();
            // trials[i] = glauber_ising_coupling(dim, 2 * beta, max_steps);
            // trials[i] = swendsen_ising_magnetization(dim, 2 * beta, max_steps);
            trials[i] = glauber_potts_coupling(dim, 2 * beta, max_steps, colors);
            diff = chrono::steady_clock::now() - start;
            times.push_back(diff.count());
        }
        sort(trials.begin(), trials.end());
        sort(times.begin(), times.end());
        med_steps = trials[trials.size() / 2];
        med_time = times[times.size() / 2];
        if (med_steps < max_steps) {
            printf("beta %f mixed in %d steps %f ms\n", beta, med_steps, med_time);
            file << beta << ", " << med_steps << ", " << fixed << med_time << "\n";
        } else {
            printf("beta %f failed to converge\n", beta);
        }
        beta += step_size;
    };

    file.close();
    return 0;
}