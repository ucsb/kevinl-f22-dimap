#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include "utils.hpp"

class Grid {
public:
    Grid();
    Grid(unsigned dim, unsigned char colors);
    Grid(unsigned w, unsigned h, unsigned char colors);
    ~Grid();
    void set(unsigned index, unsigned char new_color);
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

    unsigned w, h, size;
    unsigned char colors;
    unsigned char* graph;
    unsigned* counts;
};

unsigned choose_point(const Grid& c);

void sum_neighbors(const Grid& g, int index, unsigned counts[]);

void print_array(const Grid grids[], unsigned size);

void print_array(const Grid** grids, unsigned size);

void print_array(std::ostream& os, const Grid grids[], unsigned size);

void print_array(std::ostream& os, const Grid** grids, unsigned size);

bool mag_diff(const Grid& g1, const Grid& g2);

#endif
