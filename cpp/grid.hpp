#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <cstring>

class Grid {
public:
    Grid();
    Grid(int dim, int colors);
    Grid(int w, int h, int colors);
    ~Grid();
    void set(int index, int new_val);
    void set_all(int color);
    void chessboard();
    void print();
    Grid& operator=(const Grid& other);
    bool operator==(const Grid& other);
    bool operator!=(const Grid& other);

    int w, h, size, colors;
    int* graph;
    int* counts;
};

#endif