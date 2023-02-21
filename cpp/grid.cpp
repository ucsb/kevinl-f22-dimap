#include "grid.hpp"

Grid::Grid() : Grid(1, 1) {}
Grid::Grid(int dim, int colors) : Grid(dim, dim, colors) {}
Grid::Grid(int w, int h, int colors) {
    this->w = w;
    this->h = h;
    this->size = w * h;
    this->graph = new int[size];
    this->colors = colors;
    this->counts = new int[colors] {0};
    this->counts[0] = this->size;
}

Grid::~Grid() {
    delete[] graph;
    delete[] counts;
}

void Grid::set(int index, int new_val) {
    int old_val = this->graph[index];
    if (old_val != new_val) {
        this->counts[old_val]--;
        this->counts[new_val]++;
    }
    this->graph[index] = new_val;
}

void Grid::set_all(int color) {
    for (int i = 0; i < this->size; i++)
        this->graph[i] = color;
    for (int i = 0; i < this->colors; i++)
        this->counts[i] = 0;
    this->counts[color] = this->size;
}

void Grid::chessboard() {
    int start_color;
    for (int i = 0; i < this->colors; i++)
        counts[i] = 0;
    for (int i = 0; i < this->h; i++) {
        for (int j = 0; j < this->w; j++) {
            this->graph[(i * this->w) + j] = (i + j) % this->colors;
            this->counts[(i + j) % this->colors]++;
        }
    }
}

void Grid::print() {
    for (int i = 0; i < this->size; i++) {
        std::cout << this->graph[i] << " ";
        if (i % this->w == this->w - 1)
            std::cout << "\n";
    }
    std::cout << "Counts: ";
    for (int i = 0; i < this->colors; i++) {
        std::cout << counts[i] << " ";
    }
    std::cout << "\n";
}

Grid& Grid::operator=(const Grid& other) {
    this->w = other.w;
    this->h = other.w;
    this->size = other.size;
    this->colors = other.colors;
    delete[] this->graph;
    delete[] this->counts;
    this->graph = new int[this->size];
    this->counts = new int[this->colors];
    memcpy(this->graph, other.graph, sizeof(int) * this->size);
    memcpy(this->counts, other.counts, sizeof(int) * this->colors);
    return *this;
}

bool Grid::operator==(const Grid& other) {
    if (this->w != other.w || this->h != other.w) return false;
    if (this->colors != other.colors) return false;
    for (int i = 0; i < this->size; i++) {
        if (this->graph[i] != other.graph[i])
            return false;
    }
    return true;
}

bool Grid::operator!=(const Grid& other) {
    return !(*this == other);
}
