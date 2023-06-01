#include "grid.hpp"

Grid::Grid() : Grid(1, 1) {}
Grid::Grid(int dim, color_t colors) : Grid(dim, dim, colors) {}
Grid::Grid(int w, int h, color_t colors) {
    this->w = w;
    this->h = h;
    this->size = w * h;
    this->graph = new color_t[size];
    std::fill(graph, graph + this->size, 0);
    this->colors = colors;
    this->counts = new int[colors];
    std::fill(counts, counts + this->colors, 0);
    this->counts[0] = this->size;
}

Grid::~Grid() {
    delete[] graph;
    delete[] counts;
}

void Grid::set(int index, color_t new_color)
{
    color_t old_color = graph[index];
    if (old_color != new_color)
    {
        counts[old_color]--;
        counts[new_color]++;
        graph[index] = new_color;
    }
}

void Grid::set_all(color_t color) {
    std::fill(graph, graph + this->size, color);
    std::fill(counts, counts + this->colors, 0);
    this->counts[color] = this->size;
}

void Grid::chessboard() {
    std::fill(counts, counts + this->colors, 0);
    for (int i = 0; i < this->h; i++) {
        for (int j = 0; j < this->w; j++) {
            this->graph[(i * this->w) + j] = (i + j) % this->colors;
            this->counts[(i + j) % this->colors]++;
        }
    }
}

void Grid::rand() {
    std::mt19937 c_generator{std::random_device{}()};
    std::uniform_int_distribution<> rand_color(0, colors - 1);

    set_all(0);
    for (int i = 0; i < size; i++) {
        set(i, rand_color(c_generator));
    }
}

void Grid::print() const {
    print(std::cout);
}

void Grid::print(std::ostream& os) const {
    for (int i = 0; i < this->size; i++) {
        os << (int)(this->graph[i]) << " ";
        if (i % this->w == this->w - 1)
            os << "\n";
    }
    print_counts(os);
}

void Grid::print_counts() const {
    print_counts(std::cout);
}

void Grid::print_counts(std::ostream& os) const {
    os << "Counts: ";
    for (color_t i = 0; i < this->colors; i++) {
        os << counts[i] << " ";
    }
    os << "\n";
}

Grid& Grid::operator=(const Grid& other) {
    this->w = other.w;
    this->h = other.w;
    this->size = other.size;
    this->colors = other.colors;
    delete[] this->graph;
    delete[] this->counts;
    this->graph = new color_t[this->size];
    this->counts = new int[this->colors];
    memcpy(this->graph, other.graph, sizeof(color_t) * this->size);
    memcpy(this->counts, other.counts, sizeof(int) * this->colors);

    return *this;
}

bool Grid::operator==(const Grid& other) const
{
    if (this->w != other.w || this->h != other.w) return false;
    if (this->colors != other.colors) return false;
    for (int i = 0; i < this->size; i++) {
        if (this->graph[i] != other.graph[i])
            return false;
    }
    return true;
}

bool Grid::operator!=(const Grid& other) const
{
    return !(*this == other);
}

Lattice::Lattice() : Lattice(1, 2) {}

Lattice::Lattice(int dim, color_t colors) : Lattice(dim, dim, colors) {}

Lattice::Lattice(int w, int h, color_t colors) : Grid(w, h, colors)
{
    neighbors = new int*[size];
    for (int i = 0; i < size; i++)
    {
        neighbors[i] = new int[4];

        int prior_rows = i / w * w;
        neighbors[i][0] = mod(i + w, size);
        neighbors[i][1] = mod(i - w, size);
        neighbors[i][2] = prior_rows + mod(i - 1, w);
        neighbors[i][3] = prior_rows + mod(i + 1, w);
    }
}

Lattice::~Lattice()
{
    for (int i = 0; i < size; i++)
    {
        delete[] neighbors[i];
    }
    delete[] neighbors;
}

void Lattice::sum_neighbors_fast(int& index, color_t& c1, color_t& c2, int& sum1, int& sum2) const
{
    color_t neighbor_color;
    for (int n = 0; n < 4; n++)
    {
        neighbor_color = graph[neighbors[index][n]];
        if (neighbor_color == c1)
        {
            sum1 += 1;
        }
        else if (neighbor_color == c2)
        {
            sum2 += 1;
        }
    }
}

void Lattice::sum_neighbors_fast(int& index, int* sums) const
{
    for (int n = 0; n < 4; n++)
    {
        sums[graph[neighbors[index][n]]] += 1;
    }
}

Lattice& Lattice::operator=(const Lattice& other)
{
    for (int i = 0; i < this->size; i++)
    {
        delete[] this->neighbors[i];
    }
    this->w = other.w;
    this->h = other.w;
    this->size = other.size;
    this->colors = other.colors;
    delete[] this->graph;
    delete[] this->counts;
    delete[] this->neighbors;
    this->graph = new color_t[this->size];
    this->counts = new int[this->colors];
    this->neighbors = new int*[this->size];
    memcpy(this->graph, other.graph, sizeof(color_t) * this->size);
    memcpy(this->counts, other.counts, sizeof(int) * this->colors);
    for (int i = 0; i < this->size; i++)
    {
        this->neighbors[i] = new int[4];
        memcpy(this->neighbors[i], other.neighbors[i], sizeof(int) * 4);
    }
    return *this;
}

void sum_neighbors(const Grid& g, int index, int sums[])
{
    int prior_rows = index / g.w * g.w;
    sums[g.graph[mod(index + g.w, g.size)]]++;
    sums[g.graph[mod(index - g.w, g.size)]]++;
    sums[g.graph[prior_rows + mod(index - 1, g.w)]]++;
    sums[g.graph[prior_rows + mod(index + 1, g.w)]]++;
}

bool tot_mag(const Grid& g1, const Grid& g2) {
    int sum1 = 0, sum2 = 0, c;
    for (c = 0; c < g1.colors; c++)
        sum1 += (c * g1.counts[c]);
    for (c = 0; c < g2.colors; c++)
        sum2 += (c * g2.counts[c]);
    return (sum1 == sum2);
}

bool mag_diff(const Grid& g1, const Grid& g2) {
    if (g1.colors != g2.colors)
        return true;

    int* counts1 = new int[g1.colors];
    int* counts2 = new int[g2.colors];
    memcpy(counts1, g1.counts, sizeof(int) * g1.colors);
    memcpy(counts2, g2.counts, sizeof(int) * g2.colors);

    std::sort(counts1, counts1 + g1.colors);
    std::sort(counts2, counts2 + g2.colors);

    for (int c = 0; c < g1.colors; c++) {
        if (abs(counts1[c] - counts2[c]) > sqrt(g1.size / g1.colors)) {
            delete[] counts1;
            delete[] counts2;
            return true;
        }
    }

    delete[] counts1;
    delete[] counts2;

    return false;
}

bool mag_diff(const Grid grids[], int len) {
    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (mag_diff(grids[i], grids[j]))
                return true;
        }
    }
    return false;
}
