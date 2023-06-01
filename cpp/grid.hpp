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
    bool operator==(const Grid& other) const;
    bool operator!=(const Grid& other) const;

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
    void sum_neighbors_fast(int& index, int* sums) const;
    Lattice& operator=(const Lattice& other);

    int** neighbors;
};

void sum_neighbors(const Grid& g, int index, int counts[]);

bool tot_mag(const Grid& g1, const Grid& g2);

bool mag_diff(const Grid& g1, const Grid& g2);

bool mag_diff(const Grid grids[], int len);

template <typename T>
bool counts_diff(const T& g1, const T& g2)
{
    if (g1.colors != g2.colors)
        return true;
    for (color_t c = 0; c < g1.colors; c++)
    {
        if (g1.counts[c] != g2.counts[c])
        {
            return true;
        }
    }
    return false;
}

template <typename T>
bool counts_diff_sorted(const T grids[], int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            if (counts_diff_sorted(grids[i], grids[j]))
                return true;
        }
    }
    return false;
}

template <typename T>
bool counts_diff_sorted(const T& g1, const T& g2)
{
    if (g1.colors != g2.colors)
        return true;

    bool return_me = false;
    int* counts1 = new int[g1.colors];
    int* counts2 = new int[g2.colors];
    memcpy(counts1, g1.counts, sizeof(int) * g1.colors);
    memcpy(counts2, g2.counts, sizeof(int) * g2.colors);

    std::sort(counts1, counts1 + g1.colors);
    std::sort(counts2, counts2 + g2.colors);

    for (int c = 0; c < g1.colors; c++)
    {
        if (counts1[c] != counts2[c])
        {
            return_me = true;
            break;
        }
    }

    delete[] counts1;
    delete[] counts2;

    return return_me;
}

template <typename T>
bool counts_diff(const T grids[], int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            if (counts_diff(grids[i], grids[j]))
                return true;
        }
    }
    return false;
}

template <typename T>
bool counts_diff(const T grids[], int size, bool (*counts_diff)(const T&, const T&))
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            if (counts_diff(grids[i], grids[j]))
                return true;
        }
    }
    return false;
}

template <typename T>
bool grids_diff(const T grids[], int size)
{
    for (int i = 1; i < size; i++)
    {
        if (grids[0] != grids[i])
        {
            return true;
        }
    }
    return false;
}

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
