#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include "utils.hpp"

class Grid {
public:
    Grid();
    Grid(int dim, unsigned char colors);
    Grid(int w, int h, unsigned char colors);
    ~Grid();
    void set(int index, unsigned char new_color);
    void set_all(unsigned char color);
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
    unsigned char* graph;
    int* counts;
};

int choose_point(const Grid& c);

void sum_neighbors(const Grid& g, int index, int counts[]);

void print_array(const Grid grids[], int size);

void print_array(const Grid** grids, int size);

void print_array(std::ostream& os, const Grid grids[], int size);

void print_array(std::ostream& os, const Grid** grids, int size);

bool mag_diff(const Grid& g1, const Grid& g2);

#endif
