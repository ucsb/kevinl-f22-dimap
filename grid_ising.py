import utils
from math import exp
import random
import numpy as np

def heat_bath_flip(grid, beta, params):
    point, rand, spin = params
    y,x = point
    sums = utils.sum_neighbors(grid, point)
    beta_p = 2.0 * beta * sums[1]
    beta_n = 2.0 * beta * sums[0]
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    grid[y,x] = (rand <= prob_p)

def metropolis_flip(grid, beta, params):
    point, rand, new_spin = params
    y,x = point
    sums = utils.sum_neighbors(grid, point)
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
        ones = np.full(self.shape, 1, dtype=np.int8)
        zeros = np.full(self.shape, 0, dtype=np.int8)
        while steps < self.max_iters and not np.array_equal(ones, zeros):
            params = utils.coupling_params(self.shape)
            self.flip(ones, beta, params)
            self.flip(zeros, beta, params)
            steps += 1
        return steps, np.array_equal(ones, zeros)

class CFTP:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
    def run(self, beta):
        params_t = []
        params_t.append(utils.coupling_params(self.shape))
        while True:
            ones = np.full(self.shape, 1, dtype=np.int8)
            zeroes = np.full(self.shape, 0, dtype=np.int8)
            for t in range(len(params_t) - 1, -1, -1):
                self.flip(ones, beta, params_t[t])
                self.flip(zeroes, beta, params_t[t])
            if len(params_t) >= self.max_iters or np.array_equal(ones, zeroes):
                return len(params_t), np.array_equal(ones, zeroes)
            for i in range(len(params_t)):
                params_t.append(utils.coupling_params(self.shape))

if __name__ == "__main__":
    import sys
    import matplotlib.pyplot as plt

    if len(sys.argv) != 4:
        sys.exit("Usage: grid_ising.py grid_dim step_size max_iters")

    dim = int(sys.argv[1])
    shape = (dim, dim)
    step_size = float(sys.argv[2])
    max_iters = int(sys.argv[3])

    selection = [Forward_Coupling, CFTP]
    user_in = input("Choose a mixing time metric: (1) forward coupling (2) CFTP\n")
    mixer = selection[int(user_in) - 1]

    selection = [heat_bath_flip, metropolis_flip]
    user_in = input("Choose an algorithm: (1) heat bath (2) Metropolis filter\n")
    alg = selection[int(user_in) - 1]

    random.seed(None)

    b_crit = 0.44
    bvals = []
    bsteps = []
    bvals, bsteps = utils.simulate(mixer(alg, shape, max_iters), step_size)

    user_in = input("The simulation is done. Enter a title for the plot:\n")
    plt.plot(bvals, bsteps)
    plt.xlabel("Temperature β")
    plt.ylabel("Steps to converge (in MC steps)")
    plt.title(user_in + " ({:d}x{:d} grid)".format(dim, dim))
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
    plt.legend(loc="upper left")
    plt.savefig("grid_ising{:d}x{:d}.png".format(dim, dim))
