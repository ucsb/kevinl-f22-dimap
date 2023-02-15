#include <cstdio>
#include <vector>
#include <iostream>

using namespace std;

class Grid {
public:
    Grid(int w, int h, int colors) {
        this->w = w;
        this->h = h;
        this->size = w * h;
        this->graph = new int*[h];
        for (int i = 0; i < w; i++)
            this->graph[i] = new int[w] {0};
        this->colors = colors;
        this->counts = new int[colors] {0};
        this->counts[0] = this->size;
    }
    ~Grid() {
        for (int i = 0; i < this->w; i++)
            delete[] graph[i];
        delete[] graph;
        delete[] counts;
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

    int w, h, size, colors;
    int** graph;
    int* counts;
};

int main(int argc, char** argv) {
    Grid g(5, 5, 5);
    g.print();
    g.chessboard();
    g.print();
    return 0;
}