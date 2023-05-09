#include "chain.hpp"
#include "grid.hpp"

class Heat_Bath_Glauber_Grid : public Chain
{
public:
    Heat_Bath_Glauber_Grid() : Heat_Bath_Glauber_Grid(1) {}
    Heat_Bath_Glauber_Grid(int dim) : Chain(dim, 2) {}
    ~Heat_Bath_Glauber_Grid() = default;
    virtual int run(float beta) override;
    void flip(Grid& g, float beta, int index, float rand, float* probs);
};

class Heat_Bath_CFTP_Grid: public Heat_Bath_Glauber_Grid
{
public:
    Heat_Bath_CFTP_Grid() : Heat_Bath_CFTP_Grid(1) {}
    Heat_Bath_CFTP_Grid(int dim) : Heat_Bath_Glauber_Grid(dim) {}
    int run(float beta) override;
};

class Heat_Bath_Glauber_Complete : public Chain
{
public:
    Heat_Bath_Glauber_Complete() : Heat_Bath_Glauber_Complete(1) {}
    Heat_Bath_Glauber_Complete(int dim) : Chain(dim, 2) {}
    ~Heat_Bath_Glauber_Complete() = default;
    virtual int run(float beta) override;
    void flip(Grid& g, float beta, int index, float rand);
};

class Heat_Bath_CFTP_Complete : public Heat_Bath_Glauber_Complete
{
public:
    Heat_Bath_CFTP_Complete() : Heat_Bath_CFTP_Complete(1) {}
    Heat_Bath_CFTP_Complete(int dim) : Heat_Bath_Glauber_Complete(dim) {}
    int run(float beta) override;
};
