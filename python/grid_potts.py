import utils
from math import exp, log, sqrt
import random
import numpy as np

def sum_neighbors(grid, point):
    h,w = grid.shape
    y,x = point
    sums = [0] * grid.colors
    sums[grid[(y - 1) % h, x]] += 1
    sums[grid[(y + 1) % h, x]] += 1
    sums[grid[y, (x - 1) % w]] += 1
    sums[grid[y, (x + 1) % w]] += 1
    return sums

def choose_spin(colors):
    return int(random.uniform(0.0, 1.0) * colors)

def grids_eq(grids):
    for i in range(1, len(grids)):
        if grids[0] != grids[i]:
            return False
    return True

def heat_bath_flip(grid, beta, params):
    point, rand, spin = params
    y,x = point
    sums = sum_neighbors(grid, point)
    Z = 0
    for c in range(len(sums)):
        Z += exp(sums[c] * beta)
        sums[c] = Z
    for c in range(len(sums)):
        if rand <= (sums[c] / Z):
            grid[y,x] = c
            return

def metropolis_flip(grid, beta, params):
    point, rand, new_spin = params
    y,x = point
    sums = sum_neighbors(grid, point)
    # Index into neighbor sums based on spin
    mchrome_before = sums[grid[y,x]]
    mchrome_after = sums[new_spin]
    # Calculate acceptance probability
    prob_accept = min(1, exp((mchrome_after - mchrome_before) * beta))
    # Accept new state with calculated probability
    if rand <= prob_accept:
        grid[y,x] = new_spin

class Forward_Coupling:
    def __init__(self, flip, colors, shape, max_iters):
        self.flip = flip
        self.colors = colors
        self.shape = shape
        self.max_iters = max_iters
    def run(self, beta):
        steps = 0
        grids = []
        for c in range(self.colors):
            grids.append(utils.Potts_Grid(self.shape, c, self.colors))
        beta *= 2
        while steps < self.max_iters and not grids_eq(grids):
            params = (utils.choose_point(self.shape), random.uniform(0.0, 1.0), choose_spin(self.colors))
            for g in grids:
                self.flip(g, beta, params)
            steps += 1
        return steps, False # Second value is not used, should phase out at some point

class Magnetization:
    def __init__(self, flip, colors, shape, max_steps):
        self.flip = flip
        self.colors = colors
        self.shape = shape
        self.max_iters = max_steps
        self.target = int(1.0 / colors * shape[0] * shape[1])
    def run(self, beta):
        steps = 0
        ones = utils.Potts_Grid(self.shape, 1, self.colors)
        beta *= 2
        def converged():
            for c in ones.counts:
                if abs(c - self.target) > 1:
                    return False
            return True
        while steps < self.max_iters and not converged():
            params = (utils.choose_point(self.shape), random.uniform(0.0, 1.0), choose_spin(self.colors))
            self.flip(ones, beta, params)
            steps += 1
        return steps, False

if __name__ == "__main__":
    import sys
    import matplotlib.pyplot as plt

    if len(sys.argv) != 5:
        sys.exit("Usage: grid_potts.py colors grid_dim step_size max_steps")

    colors = int(sys.argv[1])
    if colors > 255 or colors < 2:
        exit("Only 2-255 colors supported\n")
    dim = int(sys.argv[2])
    shape = (dim, dim)
    step_size = float(sys.argv[3])
    max_steps = int(sys.argv[4])

    selection = [Forward_Coupling, Magnetization]
    user_in = input("Choose a mixing time metric: (1) forward coupling (2) magnetization\n")
    mixer = selection[int(user_in) - 1]

    selection = [heat_bath_flip, metropolis_flip]
    user_in = input("Choose an algorithm: (1) heat bath (2) Metropolis filter\n")
    alg = selection[int(user_in) - 1]

    random.seed(None)

    b_crit = log(1+sqrt(colors)) / 2
    print("Critical β for {:d} colors: {:.2f}".format(colors, b_crit))
    bvals, bsteps = utils.simulate(mixer(alg, colors, shape, max_steps), step_size)

    user_in = input("The simulation is done. Enter a title for the plot:\n")
    plt.plot(bvals, bsteps)
    plt.xlabel("Temperature β")
    plt.ylabel("Steps to converge (in MC steps)")
    plt.title(user_in + " ({:d} colors, {:d}x{:d} grid)".format(colors, dim, dim))
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
    plt.legend(loc="upper left")
    plt.savefig("grid_potts(q{:d}){:d}x{:d}.png".format(colors, dim, dim))
