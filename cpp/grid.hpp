#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include "utils.hpp"

class Grid {
public:
    Grid();
    Grid(int dim, color_t colors);
    Grid(int w, int h, color_t colors);
    ~Grid();
    void set(int index, color_t new_color);
    void set_all(color_t color);
    void chessboard();
    void rand();
    void print() const;
    void print(std::ostream& os) const;
    void print_counts() const;
    void print_counts(std::ostream& os) const;
    Grid& operator=(const Grid& other);
    bool operator==(const Grid& other);
    bool operator!=(const Grid& other);

    int w, h, size;
    int colors;
    color_t* graph;
    int* counts;
};

// Specialized class with pre-computed neighbor indices
class Lattice : public Grid
{
public:
    Lattice();
    Lattice(int dim, color_t colors);
    Lattice(int w, int h, color_t colors);
    ~Lattice();
    void sum_neighbors_fast(int& index, color_t& c1, color_t& c2, int& sum1, int& sum2) const;
    Lattice& operator=(const Lattice& other);

    int** neighbors;
};

void sum_neighbors(const Grid& g, int index, int counts[]);

bool tot_mag(const Grid& g1, const Grid& g2);

bool mag_diff(const Grid& g1, const Grid& g2);

bool mag_diff(const Grid grids[], int len);

bool counts_diff(const Grid& g1, const Grid& g2);

bool counts_diff_sorted(const Grid& g1, const Grid& g2);

bool counts_diff(const Grid grids[], int size);

bool counts_diff(const Grid grids[], int size, bool (*counts_diff)(const Grid&, const Grid&));

template <typename T>
void print_grid_array(const T grids[], int size)
{
    print_grid_array(std::cout, grids, size);
}

template <typename T>
void print_grid_array(std::ostream& os, const T grids[], int size)
{
    int max_w = 0, max_h = 0, grid_w, grid_h;
    for (int i = 0; i < size; i++) {
        if (grids[i].w > max_w) max_w = grids[i].w;
        if (grids[i].w > max_h) max_h = grids[i].h;
    }

    for (int h = 0; h < max_h; h++) {
        for (int i = 0; i < size; i++) {
            grid_w = grids[i].w;
            grid_h = grids[i].h;
            for (int w = 0; w < max_w; w++) {
                if (w < grid_w && h < grid_h) {
                    os << (int)(grids[i].graph[h * grid_w + w]) << ' ';
                }
            }
            os << ' ';
        }
        os << '\n';
    }

    for (int i = 0; i < size; i++) {
        os << "grid " << &(grids[i]) << " (graph " << (void*) grids[i].graph << ") ";
        grids[i].print_counts();
    }
}

#endif
