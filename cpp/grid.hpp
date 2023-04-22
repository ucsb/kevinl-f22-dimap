#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include "utils.hpp"

class Grid {
public:
    Grid();
    Grid(size_t dim, unsigned char colors);
    Grid(size_t w, size_t h, unsigned char colors);
    ~Grid();
    void set(size_t index, unsigned char new_color);
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

    size_t w, h, size;
    unsigned char colors;
    unsigned char* graph;
    size_t* counts;
};

size_t choose_point(const Grid& c);

void sum_neighbors(const Grid& g, size_t index, size_t counts[]);

void print_array(const Grid grids[], size_t size);

void print_array(std::ostream& os, const Grid grids[], size_t size);

bool mag_diff(const Grid& g1, const Grid& g2);

#endif
