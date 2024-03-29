#include "chain.hpp"
#include "grid.hpp"
#include "staticq.hpp"
#include <chrono>

class Swendsen_Wang_Grid : public Chain
{
public:
    Swendsen_Wang_Grid() : Swendsen_Wang_Grid(1, 2) {}
    Swendsen_Wang_Grid(int dim, color_t colors) : Chain(dim, colors) {}
    ~Swendsen_Wang_Grid() {}
    int run(float beta) override;
    void flip(Grid& g, float beta, color_t* recolors, float** probs);
};

class Swendsen_Wang_Complete : public Chain
{
public:
    Swendsen_Wang_Complete() : Swendsen_Wang_Complete(1, 2) {}
    Swendsen_Wang_Complete(int dim, color_t colors) : Chain(dim, colors) {}
    ~Swendsen_Wang_Complete() {}
    int run(float beta) override;
    int run_mag(float beta, int steps);
    void flip(Grid& g, float beta, int* recolors, float** probs, bool** rerand);
    void flip(Grid& g, float beta);
private:
    std::mt19937 c_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_color{0, colors - 1};
    std::uniform_real_distribution<float> rand_prob{0.0, 1.0};
};

class Wolff_Complete : public Chain
{
public:
    Wolff_Complete() : Wolff_Complete(1, 2) {}
    Wolff_Complete(int dim, color_t colors) : Chain(dim, colors) {}
    ~Wolff_Complete() {}
    int run(float beta) override;
    void flip(Grid& g, float beta, int start_index, color_t* recolors, float** probs, bool init);
private:
    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 c_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_index{0, size - 1};
    std::uniform_int_distribution<> rand_color{0, colors - 1};
    std::uniform_real_distribution<float> rand_prob{0.0, 1.0};
};
