#include "chain.hpp"
#include "grid.hpp"

class Metropolis_Glauber_Grid : public Chain {
public:
    Metropolis_Glauber_Grid() : Metropolis_Glauber_Grid(1) {}
    Metropolis_Glauber_Grid(int dim) : Chain(dim, 2) {}
    ~Metropolis_Glauber_Grid() = default;
    virtual int run(float beta) override;
    void flip(Grid& g, float beta, int index, color_t new_color, float rand);
};

class Metropolis_CFTP_Grid: public Metropolis_Glauber_Grid {
public:
    Metropolis_CFTP_Grid() : Metropolis_CFTP_Grid(1) {}
    Metropolis_CFTP_Grid(int dim) : Metropolis_Glauber_Grid(dim) {}
    int run(float beta) override;
};

class Metropolis_Glauber_Complete : public Chain {
public:
    Metropolis_Glauber_Complete() : Metropolis_Glauber_Complete(1) {}
    Metropolis_Glauber_Complete(int dim) : Chain(dim, 2) {}
    ~Metropolis_Glauber_Complete() {}
    virtual int run(float beta) override;
    void flip(Grid& g, float beta, int index, unsigned char new_color, float rand);
};

class Metropolis_CFTP_Complete : public Metropolis_Glauber_Complete {
public:
    Metropolis_CFTP_Complete() : Metropolis_CFTP_Complete(1) {}
    Metropolis_CFTP_Complete(int dim) : Metropolis_Glauber_Complete(dim) {}
    ~Metropolis_CFTP_Complete() {}
    virtual int run(float beta) override;
};
