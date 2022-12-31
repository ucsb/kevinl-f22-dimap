import sys
from math import exp
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

def flip(grid, beta, coords, rand):
    sums = sum_neighbors(grid, coords)
    beta_p = 2.0 * beta * sums[1]
    beta_n = 2.0 * beta * sums[0]
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    grid[coords[0], coords[1]] = (rand <= prob_p)

def side_by_side(grid1, grid2):
    if grid1.shape != grid2.shape:
        exit("Printing spins with incompatible transition matrix")
    for i in range(len(grid1)):
        print(grid1[i], grid2[i])

def simulate(shape, beta, max=None):
    point_rand = []
    while True:
        ones = np.full(shape, 1, dtype=np.int8)
        zeroes = np.full(shape, 0, dtype=np.int8)
        point = choose_point(shape=shape)
        rand = random.uniform(0.0, 1.0)
        point_rand.append((point, rand))
        for t in range(len(point_rand) - 1, -1, -1):
            # print(t, point_rand[t])
            flip(ones, beta, coords=point_rand[t][0], rand=point_rand[t][1])
            flip(zeroes, beta, coords=point_rand[t][0], rand=point_rand[t][1])
        if np.array_equal(ones, zeroes) or len(point_rand) == max:
            return len(point_rand), np.array_equal(ones, zeroes)

if len(sys.argv) != 4:
    sys.exit("Usage: glauber_ising.py grid_dim max_iters max_fails")

dim = int(sys.argv[1])
shape = (dim, dim)
max_iters = int(sys.argv[2])
max_fails = int(sys.argv[3])

random.seed(None)

# print(simulate(shape, 0, max=max_iters))

bvals = []
avgvals = []

fails = 0
for b in np.arange(0, 1, 0.02):
    avg = 0
    for i in range(10):
        steps, converged = simulate(shape, b, max=max_iters)
        if i == 0:
            avg = steps
        else:
            avg = (avg + steps) / 2
        fails += (not converged)
        if fails >= max_fails:
            break
    print("beta {:f} avg {:f} fails {:d}/{:d}".format(b, avg, fails, max_fails))
    bvals.append(b)
    avgvals.append(avg)
    if fails >= max_fails:
        break

plt.plot(bvals, avgvals)
plt.xlabel("Temperature beta")
plt.ylabel("Average Markov Chain steps")
plt.title("Ising model with coupling from the past")
plt.savefig("glauber_ising.png")