#include "swendsen_wang.hpp"

int Swendsen_Wang_Grid::run(double beta)
{
    int steps = 0;

    Lattice lattices[2];
    lattices[0] = Lattice(dim, colors);
    lattices[1] = Lattice(dim, colors);
    lattices[0].set_all(0);
    lattices[1].chessboard();

    color_t* recolors = new color_t[size];
    int** keep = new int*[size];
    for (int i = 0; i < size; i++)
    {
        keep[i] = new int[4];
    }
    double keep_edge = 1.0 - exp(-1.0 * beta);

    while (diff(lattices, 2))
    {
        for (int i = 0; i < size; i++)
        {
            recolors[i] = rand_color(generator);
            std::fill(keep[i], keep[i] + 4, -1);
        }

        for (Lattice& l : lattices)
        {
            flip(l, recolors, keep, keep_edge);
        }
        steps++;
    }

    for (int i = 0; i < size; i++)
    {
        delete[] keep[i];
    }
    delete[] keep;
    delete[] recolors;
    return steps;
}

void Swendsen_Wang_Grid::flip(Lattice& l, color_t* recolors, int** keep, double keep_edge)
{
    StaticQ<int> q(l.size);
    int index;
    color_t old_color, new_color;
    bool* visited = new bool[l.size];
    std::fill(visited, visited + l.size, 0);

    for (int i = 0; i < l.size; i++)
    {
        if (visited[i] == false)
        {
            visited[i] = true;
            q.push(i);
            old_color = l.graph[i];
            new_color = recolors[i];

            while (q.get_size() > 0)
            {
                index = *(q.pop());
                l.set(index, new_color);

                int neighbor;
                for (int j = 0; j < 4; j++)
                {
                    neighbor = l.neighbors[index][j];
                    if (visited[neighbor] == false && l.graph[neighbor] == old_color)
                    {
                        if (keep[index][j] < 0)
                        {
                            keep[index][j] = (rand_prob(generator) <= keep_edge);
                            for (int k = 0; k < 4; k++)
                            {
                                if (l.neighbors[neighbor][k] == index)
                                    keep[neighbor][k] = keep[index][j];
                            }
                        }
                        if (keep[index][j])
                        {
                            visited[neighbor] = true;
                            q.push(neighbor);
                        }
                    }
                }
            }
        }
    }
    delete[] visited;
}

int Swendsen_Wang_Complete::run(float beta)
{
    int steps = 0;

    Grid grids[2];
    grids[0] = Grid(dim, colors);
    grids[0].set_all(0);
    grids[1] = Grid(dim, colors);
    grids[1].chessboard();

    int* recolors = new int[size];
    bool** rerand = new bool*[size];
    float** probs = new float*[size];
    for (int i = 0; i < size; i++)
    {
        probs[i] = new float[size];
        rerand[i] = new bool[size];
        for (int j = 0; j < size; j++)
        {
            probs[i][j] = rand_prob(p_generator);
        }
        std::fill(rerand[i], rerand[i] + size, 0);
    }

    float beta_scaled = -1 * log(1 - (2 * beta / size));

    while (counts_diff(grids, 2, counts_diff_sorted))
    {
        std::fill(recolors, recolors + size, -1);

        for (int i = 0; i < 2; i++)
        {
            flip(grids[i], beta_scaled, recolors, probs, rerand);
        }

        // auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (rerand[i][j])
                {
                    probs[i][j] = rand_prob(p_generator);
                    rerand[i][j] = false;
                }
            }
        }
        // std::chrono::duration<float, std::milli> diff = std::chrono::steady_clock::now() - start;
        // printf("%f\n", diff.count());
        // exit(1);

        steps++;
    }

    delete[] recolors;
    for (int i = 0; i < size; i++)
    {
        delete[] probs[i];
        delete[] rerand[i];
    }
    delete[] probs;
    delete[] rerand;
    return steps;
}

int Swendsen_Wang_Complete::run_mag(float beta, int steps)
{
    Grid g(dim, colors);
    g.set_all(0);

    float beta_scaled = -1 * log(1 - (2 * beta / size));

    for (int i = 0; i < steps; i++)
    {
        flip(g, beta_scaled);
    }

    int max = -1;
    for (color_t c = 0; c < g.colors; c++)
    {
        if (g.counts[c] > max)
            max = g.counts[c];
    }

    return max;
}

void Swendsen_Wang_Complete::flip(Grid& g, float beta, int* recolors, float** probs, bool** rerand)
{
    StaticQ<int> q(g.size);
    int index;
    color_t old_color, new_color;
    bool* visited = new bool[g.size];
    std::fill(visited, visited + g.size, 0);

    float keep_edge = exp(-1 * beta);

    for (int i = 0; i < g.size; i++)
    {
        if (visited[i] == false)
        {
            if (recolors[i] < 0)
            {
                recolors[i] = rand_color(c_generator);
            }

            old_color = g.graph[i];
            new_color = recolors[i];
            visited[i] = true;
            q.push(i);

            while (q.get_size() > 0)
            {
                index = *(q.pop());
                g.set(index, new_color);

                for (int j = i + 1; j < g.size; j++)
                {
                    if (visited[j] == false && g.graph[j] == old_color)
                    {
                        if (probs[index][j] > keep_edge)
                        {
                            visited[j] = true;
                            q.push(j);
                            rerand[index][j] = true;
                        }
                    }
                }
            }
        }
    }

    delete[] visited;
}

void Swendsen_Wang_Complete::flip(Grid& g, float beta)
{
    StaticQ<int> q(g.size);
    int index;
    color_t old_color, new_color;
    bool* visited = new bool[g.size];
    std::fill(visited, visited + g.size, 0);

    float keep_edge = exp(-1 * beta);

    for (int i = 0; i < g.size; i++)
    {
        if (visited[i] == false)
        {
            old_color = g.graph[i];
            new_color = rand_color(c_generator);
            visited[i] = true;
            q.push(i);

            while (q.get_size() > 0)
            {
                index = *(q.pop());
                g.set(index, new_color);

                for (int j = i + 1; j < g.size; j++)
                {
                    if (visited[j] == false && g.graph[j] == old_color)
                    {
                        if (rand_prob(p_generator) > keep_edge)
                        {
                            visited[j] = true;
                            q.push(j);
                        }
                    }
                }
            }
        }
    }

    delete[] visited;
}

int Wolff_Complete::run(float beta)
{
    int steps = 0;

    Grid grids[2];
    grids[0] = Grid(dim, colors);
    grids[0].set_all(0);
    grids[1] = Grid(dim, colors);
    grids[1].chessboard();

    color_t* recolors = new color_t[colors];
    float** probs = new float*[size];

    for (int i = 0; i < size; i++)
    {
        probs[i] = new float[size];
    }

    float beta_scaled = -1 * log(1 - (2 * beta / size));

    while (counts_diff(grids, 2, counts_diff_sorted))
    {
        int start_index = rand_index(i_generator);
        for (color_t c = 0; c < colors; c++)
        {
            do
            {
                recolors[c] = rand_color(c_generator);
            }
            while (recolors[c] == c);
        }

        for (int i = 0; i < 2; i++)
        {
            flip(grids[i], beta_scaled, start_index, recolors, probs, (i != 0));
        }

        steps++;
    }

    delete[] recolors;
    for (int i = 0; i < grids[0].size; i++)
    {
        delete[] probs[i];
    }
    delete[] probs;
    return steps;
}

void Wolff_Complete::flip(Grid& g, float beta, int start_index, color_t* recolors, float** probs, bool init)
{
    StaticQ<int> q(g.size);
    int index;
    float keep_edge = exp(-1 * beta);

    // printf("index %d new_color %d beta %f keep_edge %f\n", start_index, new_color, beta, keep_edge);
    // g.print();

    color_t old_color = g.graph[start_index];
    color_t new_color = recolors[old_color];
    q.push(start_index);
    g.set(start_index, new_color);

    while (q.get_size() > 0)
    {
        index = *(q.pop());

        for (int j = 0; j < g.size; j++)
        {
            if (g.graph[j] == old_color) {
                if (!init)
                {
                    probs[index][j] = rand_prob(p_generator);
                }
                if (probs[index][j] > keep_edge) {
                    q.push(j);
                    g.set(j, new_color);
                }
            }
        }
    }
}
