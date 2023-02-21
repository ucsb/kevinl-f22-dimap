#include <vector>
#include <algorithm>
#include <ostream>
#include <cstdio>

#include "grid.hpp"
#include "utils.cpp"

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

class Glauber_Potts_Magnetization : public Simulation {
public:
    Glauber_Potts_Magnetization(int dim, int max_steps, int colors) : Simulation(dim, max_steps) {
        this->colors = colors;
    }
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

class Swendsen_Wang_Ising_Magnetization : public Simulation {
public:
    Swendsen_Wang_Ising_Magnetization(int dim, int max_steps) : Simulation(dim, max_steps) {}
    int run(double beta) {
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
};

void simulate(std::ostream& out, Simulation &sim, double beta_start, double beta_end, double beta_step, int runs) {
    int med_steps, steps;
    double beta = beta_start;
    double med_time;
    std::vector<int> trials;
    std::vector<double> times;

    while (med_steps < sim.max_steps && beta < beta_end) {
        trials.clear();
        times.clear();
        std::chrono::duration<double, std::milli> diff;
        for (int i = 0; i < runs; i++) {
            auto start = std::chrono::steady_clock::now();
            steps = sim.run(2 * beta);
            diff = std::chrono::steady_clock::now() - start;
            trials.push_back(steps);
            times.push_back(diff.count());
        }
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
        beta += beta_step;
    };
}
