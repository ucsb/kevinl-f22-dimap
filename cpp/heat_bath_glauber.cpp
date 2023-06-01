#include "heat_bath_glauber.hpp"

int Heat_Bath_Glauber_Grid::run(double beta)
{
    int steps = 0;
    int chains = colors + 1;

    Lattice* lattices = new Lattice[chains];
    for (color_t c = 0; c < colors; c++)
    {
        lattices[c] = Lattice(dim, colors);
        lattices[c].set_all(c);
    }
    lattices[colors] = Lattice(dim, colors);
    for (int i = 0; i < size; i++)
    {
        lattices[colors].set(i, rand_color(generator));
    }

    int index;
    double rand;

    // Precompute exp with beta
    double* exps =  new double[5];
    for (int i = 0; i < 5; i++)
    {
        exps[i] = exp(beta * i);
    }

    while (diff(lattices, chains))
    {
        for (int i = 0; i < size; i++)
        {
            index = rand_index(generator);
            rand = rand_prob(generator);
            for (color_t c = 0; c < chains; c++)
            {
                flip(lattices[c], index, rand, exps);
            }
        }
        steps += size;
    }

    delete[] exps;
    delete[] lattices;
    return steps;
}

void Heat_Bath_Glauber_Grid::flip(Lattice& l, int index, double rand, double* exps)
{
    // Count neighbors of each color for the given vertex
    int* sums = new int[colors];
    std::fill(sums, sums + colors, 0);
    l.sum_neighbors_fast(index, sums);

    // Calculate probability weight denominators
    double* weights = new double[colors];
    double sum = 0.0;
    for (color_t c = 0; c < colors; c++)
    {
        sum += exps[sums[c]];
        weights[c] = sum;
    }

    // Randomly set a new color
    double scaled_rand = rand * sum;
    for (color_t c = 0; c < colors; c++)
    {
        if (scaled_rand <= weights[c]) {
            l.set(index, c);
            break;
        }
    }

    delete[] sums;
    delete[] weights;
}

int Heat_Bath_CFTP_Grid::run(double beta)
{
    int end;
    std::vector<int> indices;
    std::vector<double> rands;
    std::vector<color_t> colors;

    Lattice lattices[2];
    for (int c = 0; c < 2; c++)
    {
        lattices[c] = Lattice(dim, 2);
        lattices[c].set_all(c);
    }

    // Precompute probabilities of flipping positive
    double* probs = new double[5];
    for (int i = 0; i < 5; i++)
    {
        probs[i] = exp(beta * i) / (exp(beta * i) + exp(beta * (4 - i)));
    }

    indices.push_back(rand_index(generator));
    rands.push_back(rand_prob(generator));

    while (diff(lattices, 2))
    {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--)
        {
            for (int c = 0; c < 2; c++)
            {
                flip(lattices[c], indices[i], rands[i], probs);
            }
            indices.push_back(rand_index(generator));
            rands.push_back(rand_prob(generator));
        }
    }

    delete[] probs;
    return (int)indices.size();
}

void Heat_Bath_CFTP_Grid::flip(Lattice& l, int index, double rand, double* probs)
{
    // Count neighbors of each color for the given vertex
    int* sums = new int[colors];
    std::fill(sums, sums + colors, 0);
    l.sum_neighbors_fast(index, sums);

    l.set(index, (rand <= probs[sums[1]]));

    delete[] sums;
}

int Heat_Bath_Glauber_Complete::run(double beta)
{
    int steps = 0;
    int chains = colors + 1;

    Grid* grids = new Grid[chains];
    for (color_t c = 0; c < colors; c++)
    {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }

    grids[colors] = Grid(dim, colors);
    for (int i = 0; i < size; i++)
    {
        grids[colors].set(i, rand_color(generator));
    }

    double beta_scaled = -1 * log(1 - (2 * beta / size));
    double* exps = new double[size + 1];
    for (int i = 0; i < size + 1; i++)
    {
        exps[i] = exp(beta_scaled * i);
    }

    int index;
    double rand;

    while (diff(grids, chains))
    {
        for (int i = 0; i < size; i++)
        {
            index = rand_index(generator);
            rand = rand_prob(generator);
            for (color_t c = 0; c < chains; c++)
            {
                flip(grids[c], index, rand, exps);
            }
        }
        steps += size;
    }

    delete[] exps;
    delete[] grids;
    return steps;
}

void Heat_Bath_Glauber_Complete::flip(Grid& g, int index, double rand, double* exps)
{
    int my_spin = g.graph[index];
    double* weights = new double[g.colors];
    double sum = 0.0;

    // Configuration weight calculations
    for (color_t c = 0; c < g.colors; c++)
    {
        sum += exps[g.counts[c] - (my_spin == c)];
        weights[c] = sum;
    }

    // Randomly set a color
    double scaled_rand = rand * sum;
    for (color_t c = 0; c < g.colors; c++)
    {
        if (scaled_rand <= weights[c]) {
            g.set(index, c);
            break;
        }
    }

    delete[] weights;
}

int Heat_Bath_CFTP_Complete::run(double beta)
{
    int end;
    std::vector<int> indices;
    std::vector<double> rands;
    std::vector<color_t> colors;

    Grid grids[2];
    for (int c = 0; c < 2; c++)
    {
        grids[c] = Grid(dim, 2);
        grids[c].set_all(c);
    }

    double beta_scaled = -1 * log(1.0 - (2.0 * beta / size));
    double* exps = new double[size + 1];
    for (int i = 0; i < size + 1; i++)
    {
        exps[i] = exp(beta_scaled * i);
    }

    indices.push_back(rand_index(generator));
    rands.push_back(rand_prob(generator));

    while (diff(grids, 2))
    {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--)
        {
            for (int c = 0; c < 2; c++)
            {
                flip(grids[c], indices[i], rands[i], exps);
            }
            indices.push_back(rand_index(generator));
            rands.push_back(rand_prob(generator));
        }
    }

    delete[] exps;
    return (int)indices.size();
}
