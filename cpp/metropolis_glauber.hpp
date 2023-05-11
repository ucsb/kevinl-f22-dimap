#include "chain.hpp"
#include "grid.hpp"

class Metropolis_Glauber_Grid : public Chain {
public:
    Metropolis_Glauber_Grid() : Metropolis_Glauber_Grid(1, 2) {}
    Metropolis_Glauber_Grid(int dim, color_t colors) : Chain(dim, colors) {}
    ~Metropolis_Glauber_Grid() = default;
    virtual int run(float beta) override;
    void run_counts(float beta, std::ofstream& os);
    void flip(Grid& g, float beta, int index, color_t new_color, float rand);
};

class Metropolis_CFTP_Grid: public Metropolis_Glauber_Grid {
public:
    Metropolis_CFTP_Grid() : Metropolis_CFTP_Grid(1) {}
    Metropolis_CFTP_Grid(int dim) : Metropolis_Glauber_Grid(dim, 2) {}
    int run(float beta) override;
};

class Metropolis_Glauber_Complete : public Chain {
public:
    Metropolis_Glauber_Complete() : Metropolis_Glauber_Complete(1, 2) {}
    Metropolis_Glauber_Complete(int dim, color_t colors) : Chain(dim, colors) {}
    ~Metropolis_Glauber_Complete() = default;
    virtual int run(float beta) override;
    void flip(Grid& g, float beta, int index, color_t new_color, float rand);
};

class Metropolis_CFTP_Complete : public Metropolis_Glauber_Complete {
public:
    Metropolis_CFTP_Complete() : Metropolis_CFTP_Complete(1) {}
    Metropolis_CFTP_Complete(int dim) : Metropolis_Glauber_Complete(dim, 2) {}
    virtual int run(float beta) override;
};
