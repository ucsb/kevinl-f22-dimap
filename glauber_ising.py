import sys
from math import exp
import matplotlib.pyplot as plt
import numpy as np
import random

if len(sys.argv) > 3 or len(sys.argv) < 2:
    sys.exit("Usage: glauber_ising.py grid_dim max_iterations(optional)")

dim = int(sys.argv[1])
shape = (dim, dim)
beta = 0.0
max_iters = None
if len(sys.argv) == 3:
    max_iters = int(sys.argv[2])

random.seed(None)

def shuffle_grid(grid, grid_dim):
    for i in range(grid_dim):
        for j in range(grid_dim):
            grid[j, i] = (int)(random.uniform(0.0, 1.0) <= 0.5)

def choose_point(grid_dim):
    x = (int)(random.uniform(0.0, 1.0) * grid_dim)
    y = (int)(random.uniform(0.0, 1.0) * grid_dim)
    return (y, x)

def sum_neighbors(grid, grid_dim, coords):
    y,x = coords
    sums = [0, 0]
    sums[grid[(y - 1) % grid_dim, x]] += 1
    sums[grid[(y + 1) % grid_dim, x]] += 1
    sums[grid[y, (x - 1) % grid_dim]] += 1
    sums[grid[y, (x + 1) % grid_dim]] += 1
    return sums

def flip(grid, grid_dim, debug=False):
    (y,x) = choose_point(grid_dim)
    sums = sum_neighbors(grid, grid_dim, (y,x)) # negative, positive
    beta_p = 2.0 * beta * sums[1]
    beta_n = 2.0 * beta * sums[0]
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    if (random.uniform(0.0, 1.0) <= prob_p):
        if debug:
            print("flip ({:d}, {:d}) {:d} -> {:d}".format(y, x, grid[y, x], 1), end='\r')
        grid[y, x] = 1
    else:
        if debug:
            print("flip ({:d}, {:d}) {:d} -> {:d}".format(y, x, grid[y, x], 0), end='\r')
        grid[y, x] = 0

def simulate(max=None, debug=False):
    grid1 = np.zeros(shape, dtype=np.int8)
    grid2 = np.ones(shape, dtype=np.int8)
    if max == None:
        steps = 0
        while not np.array_equal(grid1, grid2):
            flip(grid1, dim, debug)
            flip(grid2, dim, debug)
            steps += 1
        return steps
    else:
        for i in range(max):
            flip(grid1, dim, debug)
            flip(grid2, dim, debug)
            if np.array_equal(grid1, grid2):
                return i
        return max

bvals = []
avgvals = []

for b in np.arange(0.0, 2, 0.1):
    beta = b
    avg = 0
    for i in range(10):
        steps = simulate(max=max_iters)
        if i == 0:
            avg = steps
        else:
            avg = (avg + simulate(max=max_iters)) / 2
    print("beta {:f} avg {:f}".format(b, avg))
    bvals.append(b)
    avgvals.append(avg)

plt.plot(bvals, avgvals)
plt.xlabel("Temperature beta")
plt.ylabel("Average Markov Chain steps")
plt.title("Ising model for temperature beta")
plt.show()