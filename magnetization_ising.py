import sys
from math import exp, log
import matplotlib.pyplot as plt
import numpy as np
import random

def shuffle_grid(grid):
    h,w = grid.shape
    for i in range(w):
        for j in range(h):
            grid[j, i] = (int)(random.uniform(0.0, 1.0) <= 0.5)

def choose_point(shape):
    h,w = shape
    x = (int)(random.uniform(0.0, 1.0) * w)
    y = (int)(random.uniform(0.0, 1.0) * h)
    return (y, x)

def sum_neighbors(grid, coords):
    h,w = grid.shape
    y,x = coords
    sums = [0, 0]
    sums[grid[(y - 1) % h, x]] += 1
    sums[grid[(y + 1) % h, x]] += 1
    sums[grid[y, (x - 1) % w]] += 1
    sums[grid[y, (x + 1) % w]] += 1
    return sums

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

def flip(grid, grid_m, beta, coords, rand):
    y,x = coords
    sums = sum_neighbors(grid, coords)
    old_spin = grid[y,x]
    beta_p = 2.0 * beta * sums[1]
    beta_n = 2.0 * beta * sums[0]
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    grid[y,x] = (rand <= prob_p)
    grid_m += (2 * (grid[y,x] - old_spin)) # -2 if flip to 0, or 2 if flip to 1. Unchanged if no flip
    return grid_m

def side_by_side(grid1, grid2):
    if grid1.shape != grid2.shape:
        exit("Printing spins with incompatible transition matrix")
    for i in range(len(grid1)):
        print(grid1[i], grid2[i])

def simulate(shape, beta, max):
    steps = 0
    ones = np.full(shape, 1, dtype=np.int8)
    ones_m = sum_magnetization(ones)
    for i in range(max):
        point = choose_point(shape=shape)
        rand = random.uniform(0.0, 1.0)
        ones_m = flip(ones, ones_m, beta, coords=point, rand=rand)
    return ones_m

if len(sys.argv) != 4:
    sys.exit("Usage: magnetization_ising.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

random.seed(None)

bvals = []
avgvals = []

last_log = None
b_crit = None
vertices = dim * dim
for b in np.arange(0, 1, step_size):
    avg_mag = 0
    for i in range(10):
        mag = simulate(shape, b, max=max_iters)
        if i == 0:
            avg_mag = mag
        else:
            avg_mag = (avg_mag + mag) / 2
    avg_mag = abs(avg_mag) / vertices * 100
    if b_crit == None and last_log != None and log(avg_mag) - last_log >= 2:
        b_crit = b - step_size
    last_log = log(avg_mag)

    print("beta {:f} magnetization {:f} log {:f}".format(b, avg_mag, log(avg_mag)))
    bvals.append(b)
    avgvals.append(avg_mag)

plt.plot(bvals, avgvals)
plt.xlabel("Temperature β")
plt.ylabel("Average Magnetization")
plt.title("Glauber Heat Bath on {:d}x{:d} grid".format(dim, dim))
if b_crit != None:
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
    plt.legend(loc="upper left")
plt.savefig("magnetization{:d}x{:d}_heatbath.png".format(dim, dim))