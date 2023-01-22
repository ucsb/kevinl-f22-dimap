import sys
from math import exp
import matplotlib.pyplot as plt
import numpy as np
import random
import utils

def heat_bath_flip(grid, beta, coords, rand, new_spin):
    sums = utils.sum_neighbors(grid, coords)
    beta_p = 2.0 * beta * sums[1]
    beta_n = 2.0 * beta * sums[0]
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    grid[coords[0], coords[1]] = (rand <= prob_p)

def metropolis_flip(grid, beta, coords, rand, new_spin):
    h,w = grid.shape
    y,x = coords
    sums = utils.sum_neighbors(grid, coords)
    # Index into neighbor sums based on spin
    mchrome_before = sums[grid[y,x]]
    mchrome_after = sums[new_spin]
    # Calculate acceptance probability
    prob_accept = min(1, exp(2 * (mchrome_after - mchrome_before) * beta))
    # Accept new state with calculated probability
    if rand <= prob_accept:
        grid[y,x] = new_spin

class Forward_Coupling:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
    def run(self, beta):
        steps = 0
        ones = np.full(shape, 1, dtype=np.int8)
        zeros = np.full(shape, 0, dtype=np.int8)
        while steps < max_iters and not np.array_equal(ones, zeros):
            point = utils.choose_point(shape=shape)
            rand = random.uniform(0.0, 1.0)
            spin = utils.choose_spin(0.5)
            self.flip(ones, beta, point, rand, spin)
            self.flip(zeros, beta, point, rand, spin)
            steps += 1
        return steps, np.array_equal(ones, zeros)

if len(sys.argv) != 4:
    sys.exit("Usage: coupling_ising.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

random.seed(None)

bvals = []
bsteps = []

b_crit = 0.44
bvals, bsteps = utils.simulate(Forward_Coupling(heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Heat Bath")
bvals, bsteps = utils.simulate(Forward_Coupling(metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Metropolis Filter")
plt.xlabel("Temperature β")
plt.ylabel("Steps to converge (by forward coupling)")
plt.title("Ising model on {:d}x{:d} grid".format(dim, dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("coupling{:d}x{:d}.png".format(dim, dim))
