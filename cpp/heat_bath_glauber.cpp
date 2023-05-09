#include "heat_bath_glauber.hpp"

int Heat_Bath_Glauber_Grid::run(float beta)
{
    int steps = 0;

    Grid grids[2];
    grids[0] = Grid(dim, colors);
    grids[1] = Grid(dim, colors);
    grids[0].set_all(0);
    grids[1].set_all(1);

    // Precompute probabilities of flipping positive
    float* probs = new float[5];
    for (int i = 0; i < 5; i++)
    {
        probs[i] = exp(beta * i) / (exp(beta * i) + exp(beta * (4 - i)));
    }

    int index;
    float rand;
    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_real_distribution<float> rand_prob(0.0, 1.0);
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);

    // while (grids[0] != grids[1]) {
    while (!tot_mag(grids[0], grids[1]))
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            rand = rand_prob(p_generator);
            flip(grids[0], beta, index, rand, probs);
            flip(grids[1], beta, index, rand, probs);
        }
        steps += grids[0].size;
    }

    delete[] probs;

    return steps;
}

void Heat_Bath_Glauber_Grid::flip(Grid& g, float beta, int index, float rand, float* probs)
{
    // Count neighbors of each color for the given vertex
    int* sums = new int[g.colors];
    std::fill(sums, sums + g.colors, 0);
    sum_neighbors(g, index, sums);

    // Index into array of probabilities of flipping positive
    float prob_pos = probs[sums[1]];
    g.set(index, (rand <= prob_pos) ? 1 : 0);

    delete[] sums;
}

int Heat_Bath_CFTP_Grid::run(float beta)
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

    // Precompute probabilities of flipping positive
    float* probs = new float[5];
    for (int i = 0; i < 5; i++)
    {
        probs[i] = exp(beta * i) / (exp(beta * i) + exp(beta * (4 - i)));
    }

    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);
    std::uniform_real_distribution<float> rand_prob(0.0, 1.0);

    indices.push_back(rand_index(i_generator));
    rands.push_back(rand_prob(p_generator));

    while (!tot_mag(grids[0], grids[1]))
    {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--)
        {
            for (int c = 0; c < 2; c++)
                flip(grids[c], beta, indices[i], rands[i], probs);
            indices.push_back(rand_index(i_generator));
            rands.push_back(rand_prob(p_generator));
        }
    }

    delete[] probs;
    return (int)indices.size();
}


int Heat_Bath_Glauber_Complete::run(float beta)
{
    int steps = 0;

    Grid grids[2];
    grids[0] = Grid(dim, colors);
    grids[1] = Grid(dim, colors);
    grids[0].set_all(0);
    grids[1].set_all(1);

    float beta_scaled = -1 * log(1 - (2 * beta / grids[0].size));

    int point;
    float rand;
    std::default_random_engine generator(seed);
    std::default_random_engine generator2(seed);
    std::uniform_real_distribution<float> rand_prob(0.0, 1.0);
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);

    while (!tot_mag(grids[0], grids[1]))
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            point = rand_index(generator);
            rand = rand_prob(generator2);
            flip(grids[0], beta_scaled, point, rand);
            flip(grids[1], beta_scaled, point, rand);
        }
        steps += grids[0].size;
    }

    return steps;
}

void Heat_Bath_Glauber_Complete::flip(Grid& g, float beta, int index, float rand)
{
    int my_spin = g.graph[index];

    // Configuration weight calculations
    float exp_beta_n = exp(beta * (g.counts[0] - (my_spin == 0)));
    float exp_beta_p = exp(beta * (g.counts[1] - (my_spin == 1)));

    // Accept new state with calculated probability and update grid
    g.set(index, (rand <= exp_beta_p / (exp_beta_n + exp_beta_p)));
}

int Heat_Bath_CFTP_Complete::run(float beta)
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

    float beta_scaled = -1 * log(1 - (2 * beta / grids[0].size));

    std::default_random_engine i_generator(seed);
    std::default_random_engine r_generator(seed);
    std::uniform_real_distribution<float> rand_prob(0.0, 1.0);
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);

    indices.push_back(rand_index(i_generator));
    rands.push_back(rand_prob(r_generator));

    while (!tot_mag(grids[0], grids[1]))
    {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--)
        {
            for (int c = 0; c < 2; c++)
                flip(grids[c], beta_scaled, indices[i], rands[i]);
            indices.push_back(rand_index(i_generator));
            rands.push_back(rand_prob(r_generator));
        }
    }

    return (int)indices.size();
}
