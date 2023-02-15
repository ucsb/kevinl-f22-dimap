#include <cstdio>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);
uniform_real_distribution<double> uniform(0.0,1.0);

class Grid {
public:
    Grid(int dim, int colors) : Grid(dim, dim, colors) {}
    Grid(int w, int h, int colors) {
        this->w = w;
        this->h = h;
        this->size = w * h;
        this->graph = new int*[h];
        for (int i = 0; i < h; i++)
            this->graph[i] = new int[w] {0};
        this->colors = colors;
        this->counts = new int[colors] {0};
        this->counts[0] = this->size;
    }
    ~Grid() {
        for (int i = 0; i < this->h; i++)
            delete[] graph[i];
        delete[] graph;
        delete[] counts;
    }
    void set(int r, int c, int new_val) {
        int old_val = this->graph[r][c];
        if (old_val != new_val) {
            this->counts[old_val]--;
            this->counts[new_val]++;
        }
        this->graph[r][c] = new_val;
    }
    void set_all(int color) {
        for (int i = 0; i < this->h; i++) {
            for (int j = 0; j < this->w; j++) {
                this->graph[i][j] = color;
            }
        }
        for (int i = 0; i < this->colors; i++)
            counts[i] = 0;
        this->counts[color] = this->size;
    }
    void chessboard() {
        int start_color;
        for (int i = 0; i < this->colors; i++)
            counts[i] = 0;
        for (int i = 0; i < this->h; i++) {
            start_color = i % this->colors;
            for (int j = 0; j < this->w; j++) {
                this->graph[i][j] = (start_color + j) % this->colors;
                this->counts[(start_color + j) % this->colors]++;
            }
        }
    }
    void print() {
        for (int i = 0; i < this->h; i++) {
            for (int j = 0; j < this->w; j++) {
                if (this->graph[i][j] != -1) {
                    cout << this->graph[i][j] << " ";
                } else {
                    cout << "_ ";
                }
            }
            cout << "\n";
        }
        cout << "Counts: ";
        for (int i = 0; i < this->colors; i++) {
            cout << counts[i] << " ";
        }
        cout << "\n";
    }
    bool operator==(const Grid& other) {
        if (this->w != other.w || this->h != other.w) return false;
        if (this->colors != other.colors) return false;
        for (int i = 0; i < this->h; i++) {
            for (int j = 0; j < this->w; j++) {
                if (this->graph[i][j] != other.graph[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
    bool operator!=(const Grid& other) {
        return !(*this == other);
    }

    int w, h, size, colors;
    int** graph;
    int* counts;
};

void choose_point(int dim, int& r, int& c) {
    r = int(uniform(generator) * dim);
    c = int(uniform(generator) * dim);
}

void sum_neighbors(const Grid& g, int& r, int& c, int counts[]) {
    if (r-1 >= 0)
        counts[g.graph[r-1][c]]++;
    if (r+1 < g.h)
        counts[g.graph[r+1][c]]++;
    if (c-1 >= 0)
        counts[g.graph[r][c-1]]++;
    if (c+1 < g.w)
        counts[g.graph[r][c+1]]++;
}

void glauber_ising_flip(Grid& g, int& r, int& c, double& beta, double& rand) {
    int counts[2];
    sum_neighbors(g, r, c, counts);
    double beta_n = beta * counts[0];
    double beta_p = beta * counts[1];
    double prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p));
    g.set(r, c, (int)(rand <= prob_p));
}

int glauber_ising_coupling(int dim, double beta, int max_steps) {
    int r, c, steps = 0;
    int* counts;
    double rand;
    Grid zeros(dim, 2);
    Grid ones(dim, 2);
    zeros.set_all(0);
    ones.set_all(1);
    while (steps < max_steps && ones != zeros) {
        choose_point(dim, r, c);
        rand = uniform(generator);
        glauber_ising_flip(zeros, r, c, beta, rand);
        glauber_ising_flip(ones, r, c, beta, rand);
        steps++;
    }
    return steps;
}

int main(int argc, char** argv) {
    printf("%d steps\n", glauber_ising_coupling(100, 0, 1000000));
    return 0;
}