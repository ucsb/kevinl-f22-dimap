#include <vector>
#include <algorithm>
#include <ostream>
#include <cstdio>
#include <pthread.h>

#include "grid.hpp"
#include "utils.hpp"

class Simulation {
public:
    Simulation(int dim, int max_steps) {
        this->dim = dim;
        this->max_steps = max_steps;
    }
    virtual int run(double beta) {
        return max_steps;
    }
    int dim;
    int max_steps;
};

struct thread_arg {
    Simulation* sim;
    double beta;
    std::vector<int>* trials;
    std::vector<double>* times;
};

void* run_sim(void* arg);

class Glauber_Ising_Coupling : public Simulation {
public:
    Glauber_Ising_Coupling(int dim, int max_steps) : Simulation(dim, max_steps) {}
    int run(double beta) {
        int index, steps = 0;
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
private:
    void glauber_ising_flip(Grid& g, int& index, double& beta, double& rand) {
        int counts[2] {0};
        sum_neighbors(g, index, counts);
        double beta_n = beta * counts[0];
        double beta_p = beta * counts[1];
        double prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p));
        g.set(index, (int)(rand <= prob_p));
    }
};

class Glauber_Potts : public Simulation {
public:
    Glauber_Potts(int dim, int max_steps, int colors) : Simulation(dim, max_steps) {
        this->colors = colors;
    }
protected:
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
    int colors;
};

class Glauber_Potts_Coupling : public Glauber_Potts {
public:
    Glauber_Potts_Coupling(int dim, int max_steps, int colors) : Glauber_Potts(dim, max_steps, colors) {}
    int run(double beta) {
        int index, steps = 0;
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
private:
    bool grids_same(Grid grids[], int size) {
        for (int i = 1; i < size; i++)
            if (grids[0] != grids[i]) return false;
        return true;
    }
};

class Glauber_Potts_Magnetization : public Glauber_Potts {
public:
    Glauber_Potts_Magnetization(int dim, int max_steps, int colors) : Glauber_Potts(dim, max_steps, colors) {}
    int run(double beta) {
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
private:
    bool mag_converged(const Grid& grid, int& target) {
        for (int c = 0; c < grid.colors; c++) {
            if (abs(grid.counts[c] - target) > grid.colors)
                return false;
        }
        return true;
    }
};

class Swendsen_Wang_Ising : public Simulation {
public:
    Swendsen_Wang_Ising(int dim, int max_steps) : Simulation(dim, max_steps) {}
protected:
    void swendsen_wang_flip(Grid& g, double& beta) {
        int q_head = 0, q_tail = 0, index, old_color, new_color;
        int top, bottom, left, right;
        bool* visited = new bool[g.size] {false};
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
    void swendsen_wang_complete_flip(Grid& g, double& beta) {
        int q_head = 0, q_tail = 0, index, old_color, new_color;
        bool* visited = new bool[g.size];
        std::fill(visited, visited + g.size, 0);
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
                    for (int j = 0; j < g.size; j++) {
                        if (visited[j] == false && g.graph[j] == old_color && keep_edge(beta)) {
                            visited[j] = true;
                            queue[q_tail] = j;
                            q_tail++;
                        }
                    }
                }
            }
        }
        delete[] visited;
        delete[] queue;
    }
    bool mag_converged(const Grid& g, int& target) {
        for (int c = 0; c < g.colors; c++) {
            if (abs(g.counts[c] - target) > g.colors)
                return false;
        }
        return true;
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
};

class Swendsen_Wang_Potts_Magnetization_Complete: public Swendsen_Wang_Ising {
public:
    Swendsen_Wang_Potts_Magnetization_Complete(int dim, int max_steps, int colors) : Swendsen_Wang_Ising(dim, max_steps) {
        this->colors = colors;
    }
    int run(double beta) {
        int steps = 0;
        double beta_param = -2 * log(1 - (beta / (dim * dim)));
        Grid grids[] = {Grid(dim, colors), Grid(dim, colors)};
        grids[0].set_all(0);
        grids[1].chessboard();
        while (steps < max_steps && !mag_match(grids, 2)) {
            for (int i = 0; i < 2; i++)
                swendsen_wang_complete_flip(grids[i], beta_param);
            steps += 1;
        }
        return steps;
    }
private:
    int colors;
};

class Swendsen_Wang_Ising_Magnetization_Complete : public Swendsen_Wang_Ising {
public:
    Swendsen_Wang_Ising_Magnetization_Complete(int dim, int max_steps) : Swendsen_Wang_Ising(dim, max_steps) {}
    int run(double beta) {
        int steps = 0;
        Grid grids[] = {Grid(dim, 2), Grid(dim, 2), Grid(dim, 2)};
        grids[0].set_all(0);
        grids[1].set_all(1);
        grids[2].chessboard();
        while (steps < max_steps && !mag_match(grids, 3)) {
            for (int i = 0; i < 3; i++)
                swendsen_wang_complete_flip(grids[i], beta);
            steps += 1;
        }
        return steps;
    }
private:
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
};

class Swendsen_Wang_Ising_Magnetization : public Swendsen_Wang_Ising {
public:
    Swendsen_Wang_Ising_Magnetization(int dim, int max_steps) : Swendsen_Wang_Ising(dim, max_steps) {}
    int run (double beta) {
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
private:
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
};

void simulate(std::ostream& out, Simulation& sim, double beta_start, double beta_end, int betas, int runs);
