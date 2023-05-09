#ifndef CHAIN_H
#define CHAIN_H

#include "utils.hpp"

class Chain {
public:
    Chain() : Chain(1, 1) {}
    Chain(int dim, color_t colors) {
        this->dim = dim;
        this->size = dim * dim;
        this->colors = colors;
    }
    virtual ~Chain() {}
    virtual int run(float beta) = 0;
    virtual Chain& operator=(const Chain& other) {
        this->dim = other.dim;
        this->colors = other.colors;
        return *this;
    }
    int dim, size;
    color_t colors;
};

#endif
