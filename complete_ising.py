import utils
import random
import numpy as np
from math import exp

# Thanks to Rose and Luke for help with complete graph
def heat_bath_flip(mf, beta, params):
    point, rand, spin = params
    y,x = point
    old_spin = mf[y,x]
    beta_p = beta * (mf.p() - (old_spin == 1))
    beta_n = beta * (mf.n() - (old_spin == 0))
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    new_spin = (rand <= prob_p)
    mf[y,x] = new_spin

def metropolis_flip(mf, beta, params):
    point, rand, new_spin = params
    y,x = point
    old_spin = mf[y,x]
    sums = [mf.n(), mf.p()]
    # Index into neighbor sums based on spin
    mchrome_before = sums[old_spin] - 1
    mchrome_after = sums[new_spin] - (new_spin == old_spin)
    # Calculate acceptance probability
    prob_accept = min(1, exp((mchrome_after - mchrome_before) * beta))
    # Accept new state with calculated probability and update grid_p
    if rand <= prob_accept:
        mf[y,x] = new_spin

class Forward_Coupling:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
        self.neighbors = self.shape[0] * self.shape[1] - 1
    def run(self, beta):
        steps = 0
        ones = utils.Grid(self.shape, 1)
        zeros = utils.Grid(self.shape, 0)
        beta = 2 * beta / self.neighbors
        while steps < self.max_iters and ones != zeros:
            params = utils.coupling_params(self.shape)
            self.flip(ones, beta, params)
            self.flip(zeros, beta, params)
            steps += 1
        return steps, ones == zeros

class CFTP:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
        self.neighbors = self.shape[0] * self.shape[1] - 1
    def run(self, beta):
        params_t = []
        params_t.append(utils.coupling_params(self.shape))
        beta = 2 * beta / self.neighbors
        while True:
            ones = utils.Grid(self.shape, 1)
            zeros = utils.Grid(self.shape, 0)
            for t in range(len(params_t) - 1, -1, -1):
                self.flip(ones, beta, params_t[t])
                self.flip(zeros, beta, params_t[t])
            if len(params_t) >= self.max_iters or np.array_equal(ones, zeros):
                return len(params_t), np.array_equal(ones, zeros)
            for i in range(len(params_t)):
                params_t.append(utils.coupling_params(self.shape))

if __name__ == "__main__":
    import sys
    import matplotlib.pyplot as plt

    if len(sys.argv) != 4:
        sys.exit("Usage: complete_ising.py grid_dim step_size max_iters")

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

    b_crit = 1.0
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
    plt.savefig("complete_ising{:d}x{:d}.png".format(dim, dim))
