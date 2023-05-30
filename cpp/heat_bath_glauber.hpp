#include "chain.hpp"
#include "grid.hpp"

class Heat_Bath_Glauber_Grid : public Chain
{
public:
    Heat_Bath_Glauber_Grid() : Heat_Bath_Glauber_Grid(1, 2) {}
    Heat_Bath_Glauber_Grid(int dim, color_t colors) : Chain(dim, colors) {}
    ~Heat_Bath_Glauber_Grid() = default;
    virtual int run(double beta) override;
    void flip(Grid& g, double beta, int index, double rand);
    void flip(Lattice& l, double beta, int index, double rand);
private:
    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_real_distribution<double> rand_prob{0.0, 1.0};
    std::uniform_int_distribution<> rand_index{0, size - 1};
};

class Heat_Bath_CFTP_Grid: public Heat_Bath_Glauber_Grid
{
public:
    Heat_Bath_CFTP_Grid() : Heat_Bath_CFTP_Grid(1) {}
    Heat_Bath_CFTP_Grid(int dim) : Heat_Bath_Glauber_Grid(dim, 2) {}
    int run(double beta) override;
};

class Heat_Bath_Glauber_Complete : public Chain
{
public:
    Heat_Bath_Glauber_Complete() : Heat_Bath_Glauber_Complete(1, 2, grids_diff) {}
    Heat_Bath_Glauber_Complete(int dim, color_t colors) : Heat_Bath_Glauber_Complete(dim, colors, grids_diff) {}
    Heat_Bath_Glauber_Complete(int dim, color_t colors, bool (*diff)(const Grid[], int)) : Chain(dim, colors), diff(diff) {}
    ~Heat_Bath_Glauber_Complete() = default;
    virtual int run(double beta) override;
    void flip(Grid& g, int index, double rand, double* exps);
protected:
    bool (*diff)(const Grid[], int);
    std::mt19937 generator{std::random_device{}()};
    std::uniform_real_distribution<double> rand_prob{0.0, 1.0};
    std::uniform_int_distribution<> rand_index{0, size - 1};
    std::uniform_int_distribution<> rand_color{0, colors - 1};
};

class Heat_Bath_CFTP_Complete : public Heat_Bath_Glauber_Complete
{
public:
    Heat_Bath_CFTP_Complete() : Heat_Bath_CFTP_Complete(1) {}
    Heat_Bath_CFTP_Complete(int dim) : Heat_Bath_Glauber_Complete(dim, 2, grids_diff) {}
    int run(double beta) override;
};
