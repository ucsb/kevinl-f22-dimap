from math import exp
import numpy as np
import random
import utils

def sum_magnetization(grid):
    h,w = grid.shape
    return_me = 0
    for i in range(h):
        for j in range(w):
            if grid[i,j] == 1:
                return_me += 1
            else:
                return_me -= 1
    return return_me

def heat_bath_flip(grid, grid_m, beta, coords, rand):
    y,x = coords
    sums = utils.sum_neighbors(grid, coords)
    old_spin = grid[y,x]
    beta_p = 2.0 * beta * sums[1]
    beta_n = 2.0 * beta * sums[0]
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    grid[y,x] = (rand <= prob_p)
    # -2 if flip to 0, or 2 if flip to 1. Unchanged if no flip
    grid_m += (2 * (grid[y,x] - old_spin))
    return grid_m

def metropolis_flip(grid, grid_m, beta, coords, rand):
    y,x = coords
    sums = utils.sum_neighbors(grid, coords)
    old_spin = grid[y,x]
    new_spin = utils.choose_spin(0.5)
    # Index into neighbor sums based on spin
    mchrome_before = sums[old_spin]
    mchrome_after = sums[new_spin]
    # Calculate acceptance probability
    prob_accept = min(1, exp(2 * (mchrome_after - mchrome_before) * beta))
    # Accept new state with calculated probability
    if rand <= prob_accept:
        grid[y,x] = new_spin
    # -2 if flip to 0, or 2 if flip to 1. Unchanged if no flip
    grid_m += (2 * (grid[y,x] - old_spin))
    return grid_m

class Magnetization:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
        self.total_vertices = shape[0] * shape[1]
    def run(self, beta):
        steps = 0
        ones = np.full(self.shape, 1, dtype=np.int8)
        ones_m = sum_magnetization(ones)
        while steps < self.max_iters and (abs(ones_m) > .1):
            point = utils.choose_point(self.shape)
            rand = random.uniform(0.0, 1.0)
            ones_m = self.flip(ones, ones_m, beta, coords=point, rand=rand)
            steps += 1
        return steps, (abs(ones_m) <= .1)

if __name__ == "__main__":
    import sys
    import matplotlib.pyplot as plt

    if len(sys.argv) != 4:
        sys.exit("Usage: magnetization_ising.py grid_dim step_size max_iters")

    dim = int(sys.argv[1])
    shape = (dim, dim)
    step_size = float(sys.argv[2])
    max_iters = int(sys.argv[3])

    random.seed(None)

    bvals = []
    bsteps = []

    b_crit = 0.44
    bvals, bsteps = utils.simulate(Magnetization(heat_bath_flip, shape, max_iters), step_size)
    plt.plot(bvals, bsteps, label="Heat Bath")
    bvals, bsteps = utils.simulate(Magnetization(metropolis_flip, shape, max_iters), step_size)
    plt.plot(bvals, bsteps, label="Metropolis Filter")
    plt.xlabel("Temperature β")
    plt.ylabel("Steps to converge (by magnetization)")
    plt.title("Ising model on {:d}x{:d} grid".format(dim, dim))
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
    plt.legend(loc="upper left")
    plt.savefig("magnetization{:d}x{:d}.png".format(dim, dim))
