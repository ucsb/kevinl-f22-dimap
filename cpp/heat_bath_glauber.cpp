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
    lattices[colors].rand();

    int index;
    double rand;

    bool diff = true;
    while (diff)
    {
        for (int i = 0; i < size; i++)
        {
            index = rand_index(i_generator);
            rand = rand_prob(p_generator);
            for (color_t c = 0; c < chains; c++)
            {
                flip(lattices[c], beta, index, rand);
            }
        }
        steps += size;

        diff = false;
        for (int c = 1; c < chains; c++)
        {
            if (lattices[0] != lattices[c])
            {
                diff = true;
                break;
            }
        }
    }

    delete[] lattices;
    return steps;
}

void Heat_Bath_Glauber_Grid::flip(Grid& g, double beta, int index, double rand)
{
    // Count neighbors of each color for the given vertex
    int* sums = new int[g.colors];
    std::fill(sums, sums + g.colors, 0);
    sum_neighbors(g, index, sums);

    // Calculate probability weight denominators
    double* weights = new double[g.colors];
    double sum = 0.0;
    for (color_t c = 0; c < g.colors; c++)
    {
        sum += exp(beta * sums[c]);
        weights[c] = sum;
    }

    // Normalize weights and randomly set a new color
    sum = 1.0 / sum;
    for (color_t c = 0; c < g.colors; c++)
    {
        if (rand < (weights[c] * sum))
        {
            g.set(index, c);
            break;
        }
    }

    delete[] weights;
    delete[] sums;
}

void Heat_Bath_Glauber_Grid::flip(Lattice& l, double beta, int index, double rand)
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
        sum += exp(beta * sums[c]);
        weights[c] = sum;
    }

    // Normalize weights and randomly set a new color
    sum = 1.0 / sum;
    for (color_t c = 0; c < colors; c++)
    {
        if (rand < (weights[c] * sum))
        {
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

    Grid grids[2];
    for (int c = 0; c < 2; c++)
    {
        grids[c] = Grid(dim, 2);
        grids[c].set_all(c);
    }

    // Precompute probabilities of flipping positive
    double* probs = new double[5];
    for (int i = 0; i < 5; i++)
    {
        probs[i] = exp(beta * i) / (exp(beta * i) + exp(beta * (4 - i)));
    }

    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);
    std::uniform_real_distribution<double> rand_prob(0.0, 1.0);

    indices.push_back(rand_index(i_generator));
    rands.push_back(rand_prob(p_generator));

    while (!tot_mag(grids[0], grids[1]))
    {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--)
        {
            for (int c = 0; c < 2; c++)
                flip(grids[c], beta, indices[i], rands[i]);
            indices.push_back(rand_index(i_generator));
            rands.push_back(rand_prob(p_generator));
        }
    }

    delete[] probs;
    return (int)indices.size();
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
    grids[colors].rand();

    double beta_scaled = -1 * log(1 - (2 * beta / size));

    int index;
    double rand;

    bool diff = true;
    while (diff)
    {
        for (int i = 0; i < size; i++)
        {
            index = rand_index(i_generator);
            rand = rand_prob(p_generator);
            for (color_t c = 0; c < chains; c++)
            {
                flip(grids[c], beta_scaled, index, rand);
            }
        }
        steps += size;

        diff = false;
        for (int c = 1; c < chains; c++)
        {
            if (grids[0] != grids[c])
            {
                diff = true;
            }
        }
    }

    delete[] grids;
    return steps;
}

void Heat_Bath_Glauber_Complete::flip(Grid& g, double beta, int index, double rand)
{
    int my_spin = g.graph[index];
    double* weights = new double[g.colors];
    double sum = 0.0;

    // Configuration weight calculations
    for (color_t c = 0; c < g.colors; c++)
    {
        sum += exp(beta * (g.counts[c] - (my_spin == c)));
        weights[c] = sum;
    }

    // Normalize weights and randomly set a color
    sum = 1.0 / sum;
    for (color_t c = 0; c < g.colors; c++)
    {
        if (rand <= (weights[c] * sum)) {
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

    double beta_scaled = -1 * log(1 - (2 * beta / grids[0].size));

    std::mt19937 i_generator{std::random_device{}()};
    std::mt19937 p_generator{std::random_device{}()};
    std::uniform_real_distribution<double> rand_prob(0.0, 1.0);
    std::uniform_int_distribution<> rand_index(0, grids[0].size - 1);

    indices.push_back(rand_index(i_generator));
    rands.push_back(rand_prob(p_generator));

    while (!tot_mag(grids[0], grids[1]))
    {
        end = indices.size() - 1;
        for (int i = end; i >= 0; i--)
        {
            for (int c = 0; c < 2; c++)
                flip(grids[c], beta_scaled, indices[i], rands[i]);
            indices.push_back(rand_index(i_generator));
            rands.push_back(rand_prob(p_generator));
        }
    }

    return (int)indices.size();
}
