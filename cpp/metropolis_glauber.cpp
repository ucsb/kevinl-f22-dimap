#include "metropolis_glauber.hpp"

int Metropolis_Glauber_Grid::run(float beta)
{
    int steps = 0;

    Grid grids[2];
    grids[0] = Grid(dim, colors);
    grids[1] = Grid(dim, colors);
    grids[0].set_all(0);
    grids[1].set_all(1);

    int index;
    color_t color;
    float rand;
    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 c_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);
    std::uniform_int_distribution<> rand_color(0, grids[0].colors - 1);
    std::uniform_real_distribution<float> rand_prob(0.0, 1.0);

    while (!tot_mag(grids[0], grids[1]))
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);
            flip(grids[0], beta, index, color, rand);
            flip(grids[1], beta, index, color, rand);
        }
        steps += grids[0].size;
    }

    return steps;
}

void Metropolis_Glauber_Grid::flip(Grid& g, float beta, int index, color_t new_color, float rand)
{
    // Count neighbors of each color for the given vertex
    int sums[2];
    sums[0] = 0;
    sums[1] = 0;
    sum_neighbors(g, index, sums);

    // Index into neighbor sums based on spin
    int mchrome_before = sums[g.graph[index]];
    int mchrome_after = sums[new_color];

    // Calculate acceptance probability
    float prob_accept = std::min(1.0, exp((mchrome_after - mchrome_before) * beta));

    // Accept new state with calculated probability and update grid
    if (rand <= prob_accept)
        g.set(index, new_color);
}

int Metropolis_CFTP_Grid::run(float beta)
{
    int end;
    std::vector<int> indices;
    std::vector<float> rands;
    std::vector<color_t> colors;

    Grid grids[2];
    for (int c = 0; c < 2; c++)
    {
        grids[c] = Grid(dim, 2);
        grids[c].set_all(c);
    }

    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 c_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);
    std::uniform_int_distribution<> rand_color(0, grids[0].colors - 1);
    std::uniform_real_distribution<float> rand_prob(0.0, 1.0);

    indices.push_back(rand_index(i_generator));
    colors.push_back(rand_color(c_generator));
    rands.push_back(rand_prob(p_generator));

    while (!tot_mag(grids[0], grids[1]))
    {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--)
        {
            for (int c = 0; c < 2; c++)
                flip(grids[c], beta, indices[i], colors[i], rands[i]);
            indices.push_back(rand_index(i_generator));
            colors.push_back(rand_color(c_generator));
            rands.push_back(rand_prob(p_generator));
        }
    }

    return (int)indices.size();
}

int Metropolis_Glauber_Complete::run(float beta)
{
    int steps = 0;

    Grid grids[2];
    grids[0] = Grid(dim, colors);
    grids[1] = Grid(dim, colors);
    grids[0].set_all(0);
    grids[1].set_all(1);

    float beta_scaled = -1 * log(1 - (2 * beta / grids[0].size));

    int index;
    color_t color;
    float rand;
    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 c_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);
    std::uniform_int_distribution<> rand_color(0, grids[0].colors - 1);
    std::uniform_real_distribution<float> rand_prob(0.0, 1.0);

    while (!tot_mag(grids[0], grids[1]))
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);
            flip(grids[0], beta_scaled, index, color, rand);
            flip(grids[1], beta_scaled, index, color, rand);
        }
        steps += grids[0].size;
    }

    return steps;
}

void Metropolis_Glauber_Complete::flip(Grid& g, float beta, int index, color_t new_color, float rand)
{
    color_t old_color = g.graph[index];
    float prob_accept;

    // Calculate acceptance probability
    if (old_color == new_color)
    {
        prob_accept = 1; // exp(0)
    }
    else
    {
        int mchrome_old = 0;
        int mchrome_new = 0;

        if (g.counts[new_color] > 0)
            mchrome_new = g.counts[new_color];
        if (g.counts[old_color] > 1)
            mchrome_old = 1 - g.counts[old_color];

        prob_accept = std::min(1.0, exp((mchrome_old + mchrome_new) * beta));
    }

    // Accept new state with calculated probability and update grid
    if (rand <= prob_accept)
        g.set(index, new_color);
}

int Metropolis_CFTP_Complete::run(float beta) {
    int end;
    std::vector<int> indices;
    std::vector<float> rands;
    std::vector<unsigned char> colors;
    Grid grids[2];
    for (int c = 0; c < 2; c++) {
        grids[c] = Grid(dim, 2);
        grids[c].set_all(c);
    }

    float beta_scaled = -1 * log(1 - (beta / grids[0].size));

    indices.push_back(choose_point(grids[0]));
    rands.push_back(uniform(generator));
    colors.push_back(2*uniform(generator));
    while (grids[0] != grids[1]) {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--) {
            for (int c = 0; c < 2; c++)
                flip(grids[c], beta_scaled, indices[i], colors[i], rands[i]);
            indices.push_back(choose_point(grids[0]));
            rands.push_back(uniform(generator));
            colors.push_back(2*uniform(generator));
        }
    }
    return (int)indices.size();
}
