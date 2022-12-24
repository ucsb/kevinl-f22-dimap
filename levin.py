import numpy as np
import random
import sys
from math import exp
import matplotlib.pyplot as plt

def shuffle_grid(grid):
    h, w = grid.shape
    for i in range(w):
        for j in range(h):
            grid[j, i] = (int)(random.uniform(0.0, 1.0) <= 0.5)

def sum_neighbors(grid, coords):
    h, w = grid.shape
    y, x = coords
    sums = [0, 0]
    sums[grid[(y - 1) % h, x]] += 1
    sums[grid[(y + 1) % h, x]] += 1
    sums[grid[y, (x - 1) % w]] += 1
    sums[grid[y, (x + 1) % w]] += 1
    return sums

def choose_point(grid):
    h, w = grid.shape
    x = (int)(random.uniform(0.0, 1.0) * w)
    y = (int)(random.uniform(0.0, 1.0) * h)
    return (y, x)

def flip(grid, beta):
    point = choose_point(grid)
    sums = sum_neighbors(grid, point)
    beta_sum = ((sums[0] * -1) + sums[1]) * beta
    prob = exp(beta_sum) / (exp(beta_sum) + exp(-1 * beta_sum))
    grid[point[0], point[1]] = (random.uniform(0.0, 1.0) <= prob)

random.seed(None)

if len(sys.argv) != 3:
    exit("Usage: levin.py grid_dim beta")

grid_dim = int(sys.argv[1])
grid_shape = (grid_dim, grid_dim)
beta = float(sys.argv[2])
grid = np.full(grid_shape, 1, dtype=np.int8)


fig, ax = plt.subplots(1, 5, figsize=(15, 3))

for j in range(5):
    for i in range(50000000):
        flip(grid, beta)
    ax[j].imshow(grid, cmap='gray', vmin=0, vmax=1)
    ax[j].set_title("{:d} steps".format(10000000 * (j + 1)))
plt.savefig("levin.png")