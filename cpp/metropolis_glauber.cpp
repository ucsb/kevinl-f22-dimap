#include "metropolis_glauber.hpp"

int Metropolis_Glauber_Grid::run(float beta)
{
    switch(mixer)
    {
        case 0:
            return run_exact(beta);
        case 1:
            return run_counts(beta);
        case 2:
            return run_counts_sorted(beta);
        case 4:
            return run_gelman_rubin(beta);
        default:
            return -1;
    }
}

int Metropolis_Glauber_Grid::run_exact(float beta)
{
    int steps = 0;

    Grid* grids = new Grid[colors];
    for (color_t c = 0; c < colors; c++)
    {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }

    int index;
    color_t color;
    float rand;

    bool diff = true;
    while (diff)
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);

            for (color_t c = 0; c < colors; c++)
            {
                flip(grids[c], beta, index, color, rand);
            }
        }
        steps += grids[0].size;

        diff = false;
        for (color_t c = 1; c < colors; c++)
        {
            diff = (diff || (grids[0] != grids[c]));
        }
    }

    delete[] grids;
    return steps;
}

int Metropolis_Glauber_Grid::run_counts(float beta)
{
    int steps = 0;

    Grid* grids = new Grid[colors];
    for (color_t c = 0; c < colors; c++)
    {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }

    int index;
    color_t color;
    float rand;

    while (counts_diff(grids, colors))
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);

            for (color_t c = 0; c < colors; c++)
            {
                flip(grids[c], beta, index, color, rand);
            }
        }
        steps += grids[0].size;
    }

    delete[] grids;
    return steps;
}

int Metropolis_Glauber_Grid::run_counts_sorted(float beta)
{
    int steps = 0;

    Grid* grids = new Grid[colors + 1];
    for (color_t c = 0; c < colors + 1; c++)
    {
        grids[c] = Grid(dim, colors);

        if (c == colors)
            grids[c].chessboard();
        else
            grids[c].set_all(c);
    }

    int index;
    color_t color;
    float rand;

    while (counts_diff(grids, colors + 1, counts_diff_sorted))
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);

            for (color_t c = 0; c < colors + 1; c++)
            {
                flip(grids[c], beta, index, color, rand);
            }
        }
        steps += size;
    }

    delete[] grids;
    return steps;
}

int Metropolis_Glauber_Grid::run_gelman_rubin(float beta)
{
    long* sample_sums = new long[colors];
    std::fill(sample_sums, sample_sums + colors, 0);
    float* sample_means = new float[colors];
    float means_avg = 0.0;
    float vars_avg = 0.0;
    float means_var = 0.0;
    int steps = 0;
    int samples = 0;

    std::vector<std::vector<int>> mags(colors, std::vector<int>());

    float rhat = 2;

    Grid* grids = new Grid[colors];

    for (color_t c = 0; c < colors; c++)
    {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }

    long burn_in = 0;

    // print_grid_array(grids, colors);

    while (rhat > 1.2)
    {
        means_avg = 0;
        vars_avg = 0;

        for (int i = 0; i < grids[0].size; i++)
        {
            int index = rand_index(i_generator);
            color_t color = rand_color(c_generator);
            float rand = rand_prob(p_generator);

            for (color_t c = 0; c < colors; c++)
            {
                flip(grids[c], beta, index, color, rand);

                if (burn_in++ > 100)
                {
                    int mag = *(std::max_element(grids[c].counts, grids[c].counts + colors));

                    // printf("chain %d mag %d\n", c, mag);

                    mags[c].push_back(mag);
                    sample_sums[c] += mag;
                    samples = (int)(mags[c].size());

                    // Calculate sample mean
                    sample_means[c] = sample_sums[c] / (float)(samples);

                    printf("chain %d sample mean %f\n", c, sample_means[c]);

                    // Sample variance (about sample mean)
                    float sample_var = 0.0;
                    for (int i = 0; i < samples; i++)
                    {
                        float diff = mags[c][i] - sample_means[c];
                        // printf("chain %d %d %f diff %f\n", c, mags[c][i], sample_means[c], diff);
                        sample_var += (diff * diff);
                    }

                    printf("chain %d var %f\n", c, sample_var);

                    sample_var = sample_var / (samples - 1);
                    means_avg += sample_means[c];
                    vars_avg += sample_var;
                }
            }
        }

        steps += size;

        if (samples < 2)
            continue;

        // Average of chain means
        means_avg = means_avg / colors;

        // printf("means_avg %f\n", means_avg);

        // Average of chain variances (W)
        vars_avg = vars_avg / colors;

        // printf("vars_avg %f\n", vars_avg);

        for (color_t c = 0; c < colors; c++)
        {
            float diff = sample_means[c] - means_avg;
            means_var += (diff * diff);
        }

        // Variance of chain means around joint mean (B/L)
        means_var = means_var / (colors - 1);

        // printf("means_var %f\n", means_var);

        rhat = std::sqrt((((float)(samples - 1) / samples * vars_avg) + ((colors + 1) / ((float)colors)) * means_var) / vars_avg);
        printf("new rhat %f\n", rhat);

        // print_grid_array(grids, colors);
        // if (x++ == 10) exit(1);
    }

    exit(1);
    delete[] grids;
    delete[] sample_means;
    delete[] sample_sums;

    return steps;
}

int Metropolis_Glauber_Grid::run_mag(float beta, int max_steps)
{
    int steps = 0;

    Grid* grids = new Grid[colors];
    for (color_t c = 0; c < colors; c++)
    {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }

    int index;
    color_t color;
    float rand;

    bool diff = true;
    while (diff && steps < max_steps)
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);

            for (color_t c = 0; c < colors; c++)
            {
                flip(grids[c], beta, index, color, rand);
            }
        }
        steps += grids[0].size;

        diff = false;
        for (color_t c = 1; c < colors; c++)
        {
            diff = (diff || (grids[0] != grids[c]));
        }
    }

    int mag = *(std::max_element(grids[0].counts, grids[0].counts + colors));

    delete[] grids;
    return mag;
}

void Metropolis_Glauber_Grid::log_counts(float beta, std::ofstream& os)
{
    int steps = 0;

    Grid* grids = new Grid[colors];
    for (color_t c = 0; c < colors; c++)
    {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }

    int index;
    color_t color;
    float rand;

    while (counts_diff(grids, colors))
    {
        os << steps;
        for (color_t g = 0; g < colors; g++)
        {
            int sum = 0;
            for (color_t c = 0; c < colors; c++)
            {
                sum += (c * grids[g].counts[c]);
            }
            os << ',' << sum;
        }
        os << '\n';

        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);
            for (color_t c = 0; c < colors; c++)
            {
                flip(grids[c], beta, index, color, rand);
            }
        }
        steps += grids[0].size;
    }

    delete[] grids;
}

void Metropolis_Glauber_Grid::flip(Grid& g, float beta, int index, color_t new_color, float rand)
{
    // Count neighbors of each color for the given vertex
    int* sums = new int[colors];
    std::fill(sums, sums + colors, 0);
    sum_neighbors(g, index, sums);

    // Index into neighbor sums based on spin
    int mchrome_before = sums[g.graph[index]];
    int mchrome_after = sums[new_color];

    // Calculate acceptance probability
    float prob_accept = std::min(1.0, exp((mchrome_after - mchrome_before) * beta));

    // Accept new state with calculated probability and update grid
    if (rand <= prob_accept)
        g.set(index, new_color);

    delete[] sums;
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

    Grid* grids = new Grid[colors];
    for (color_t c = 0; c < colors; c++)
    {
        grids[c] = Grid(dim, colors);
        grids[c].set_all(c);
    }

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

    while (counts_diff(grids, colors))
    {
        for (int i = 0; i < grids[0].size; i++)
        {
            index = rand_index(i_generator);
            color = rand_color(c_generator);
            rand = rand_prob(p_generator);
            for (color_t c = 0; c < colors; c++)
            {
                flip(grids[c], beta_scaled, index, color, rand);
            }
        }
        steps += grids[0].size;
    }

    delete[] grids;
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
        /*
        If a site is flipped to a new color, there are <count> number of
        other sites which create edges with the new vertex
        */

        int mchrome_new = g.counts[new_color];

        /*
        If the site had edges to other sites, there the number of monochromatic
        edges will go down by <count> - 1 (one is the current site itself)
        */

        int mchrome_old = 0;
        if (g.counts[old_color] > 1)
            mchrome_old = 1 - g.counts[old_color];

        prob_accept = std::min(1.0, exp((mchrome_old + mchrome_new) * beta));
    }

    // Accept new state with calculated probability and update grid
    if (rand <= prob_accept)
        g.set(index, new_color);
}

int Metropolis_CFTP_Complete::run(float beta)
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
                flip(grids[c], beta_scaled, indices[i], colors[i], rands[i]);
            indices.push_back(rand_index(i_generator));
            colors.push_back(rand_color(c_generator));
            rands.push_back(rand_prob(p_generator));
        }
    }

    return (int)indices.size();
}
