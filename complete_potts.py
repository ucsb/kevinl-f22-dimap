import utils
import random
import numpy as np
from math import exp, log
from grid_potts import grids_eq, choose_spin

def heat_bath_flip(grid, beta, params):
    point, rand, spin = params
    y,x = point
    old_spin = grid[y,x]
    Z = 0
    sums = [0] * grid.colors
    for c in range(grid.colors):
        Z += exp(beta * (grid.counts[c] - (old_spin == c)))
        sums[c] = Z
    for c in range(grid.colors):
        if rand <= (sums[c] / Z):
            grid[y,x] = c
            return

def metropolis_flip(grid, beta, params):
    point, rand, new_spin = params
    old_spin = grid[point]
    # Index into neighbor sums based on spin
    mchrome_before = grid.counts[old_spin] - 1
    mchrome_after = grid.counts[new_spin] - (new_spin == old_spin)
    # Calculate acceptance probability
    prob_accept = min(1, exp((mchrome_after - mchrome_before) * beta))
    # Accept new state with calculated probability and update grid_p
    if rand <= prob_accept:
        grid[point] = new_spin

class Forward_Coupling:
    def __init__(self, flip, colors, shape, max_iters):
        self.flip = flip
        self.colors = colors
        self.shape = shape
        self.max_iters = max_iters
        self.neighbors = shape[0] * shape[1] - 1
    def run(self, beta):
        steps = 0
        grids = []
        for c in range(self.colors):
            grids.append(utils.Potts_Grid(self.shape, c, self.colors))
        beta = 2 * beta / self.neighbors
        while steps < self.max_iters and not grids_eq(grids):
            params = (utils.choose_point(shape), random.uniform(0.0, 1.0), choose_spin(self.colors))
            for g in grids:
                self.flip(g, beta, params)
            steps += 1
        return steps, False

class Magnetization:
    def __init__(self, flip, colors, shape, max_steps):
        self.flip = flip
        self.colors = colors
        self.shape = shape
        self.max_iters = max_steps
        self.neighbors = self.shape[0] * self.shape[1] - 1
        self.target = int(1.0 / colors * shape[0] * shape[1])
    def run(self, beta):
        steps = 0
        ones = utils.Potts_Grid(self.shape, 1, self.colors)
        beta = 2 * beta / self.neighbors
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

    if len(sys.argv) != 6:
        sys.exit("Usage: complete_potts.py colors grid_dim step_size max_iters b_dyn")

    colors = int(sys.argv[1])
    if colors > 255 or colors < 2:
        exit("Only 2-255 colors supported\n")
    dim = int(sys.argv[2])
    shape = (dim, dim)
    step_size = float(sys.argv[3])
    max_iters = int(sys.argv[4])
    b_dyn = float(sys.argv[5])

    selection = [Forward_Coupling, Magnetization]
    user_in = input("Choose a mixing time metric: (1) forward coupling (2) magnetization\n")
    mixer = selection[int(user_in) - 1]

    selection = [heat_bath_flip, metropolis_flip]
    user_in = input("Choose an algorithm: (1) heat bath (2) Metropolis filter\n")
    alg = selection[int(user_in) - 1]

    random.seed(None)

    # critical beta: https://iopscience.iop.org/article/10.1088/0305-4470/7/9/003/pdf
    b_crit = log(colors - 1) * (colors - 1) / (colors - 2)
    print("Critical beta for {:d} colors: {:.2f}".format(colors, b_crit))
    bvals, bsteps = utils.simulate(mixer(alg, colors, shape, max_iters), step_size)

    user_in = input("The simulation is done. Enter a title for the plot:\n")
    plt.xlim(right=(b_crit + step_size))
    plt.ylim(top=max_iters)
    plt.plot(bvals, bsteps, color="steelblue")
    plt.annotate(
        "β={:.2f}".format(bvals[-1]),
        xy=(bvals[-1], bsteps[-1]),
        textcoords="offset points",
        xytext=(0,5),
        ha='right',
        color="steelblue")
    plt.xlabel("Temperature β")
    plt.ylabel("Mixing time (in MC steps)")
    plt.title(user_in + " ({:d}x{:d} mean field)".format(dim, dim))
    plt.axvline(x = b_dyn, color = 'r', linestyle = '-', label="First critical β")
    plt.annotate(
        "β={:.2f}".format(b_dyn),
        xy=(b_dyn, max_iters),
        textcoords="offset points",
        xytext=(0, -15),
        ha='right',
        color="red")
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Second critical β")
    plt.annotate(
        "β={:.2f}".format(b_crit),
        xy=(b_crit, max_iters),
        textcoords="offset points",
        xytext=(0, -15),
        ha='right',
        color="red")
    plt.legend(loc="upper left")
    plt.savefig("complete_potts{:d}x{:d}.png".format(dim, dim))
