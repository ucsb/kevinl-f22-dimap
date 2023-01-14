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

def choose_spin(prob_pos):
    return (random.uniform(0.0, 1.0) <= prob_pos)

def sum_neighbors(grid, coords):
    h,w = grid.shape
    y,x = coords
    sums = [0, 0]
    sums[grid[(y - 1) % h, x]] += 1
    sums[grid[(y + 1) % h, x]] += 1
    sums[grid[y, (x - 1) % w]] += 1
    sums[grid[y, (x + 1) % w]] += 1
    return sums

def flip(grid, beta, coords, new_spin, rand):
    h,w = grid.shape
    y,x = coords
    sums = sum_neighbors(grid, coords)

    # Index into neighbor sums based on spin
    mchrome_before = sums[grid[y,x]]
    mchrome_after = sums[new_spin]

    # Calculate acceptance probability
    prob_accept = min(1, exp(2 * (mchrome_after - mchrome_before) * beta))

    # Accept new state with calculated probability
    if rand <= prob_accept:
        grid[y,x] = new_spin

def side_by_side(grid1, grid2):
    if grid1.shape != grid2.shape:
        exit("Printing spins with incompatible transition matrix")
    for i in range(len(grid1)):
        print(grid1[i], grid2[i])

def simulate(shape, beta, max=None):
    dim,dim2 = shape
    steps = 0
    ones = np.full(shape, 1, dtype=np.int8)
    zeros = np.full(shape, 0, dtype=np.int8)
    while not np.array_equal(ones, zeros):
        point = choose_point(shape=shape)
        spin = choose_spin(0.5)
        rand = random.uniform(0.0, 1.0)
        flip(ones, beta, point, spin, rand)
        flip(zeros, beta, point, spin, rand)
        steps += 1
        if steps == max:
            break
    return steps, np.array_equal(ones, zeros)

if len(sys.argv) != 5:
    sys.exit("Usage: glauber_ising_metropolis.py grid_dim max_iters max_fails output_name")

dim = int(sys.argv[1])
shape = (dim, dim)
max_iters = int(sys.argv[2])
max_fails = int(sys.argv[3])
output_name = sys.argv[4]

random.seed(None)

bvals = []
avgvals = []

step_size = 0.02
fails = 0
last_log = None
b_crit = None
for b in np.arange(0, 1, step_size):
    avg_steps = 0
    for i in range(10):
        steps, converged = simulate(shape, b, max=max_iters)
        if i == 0:
            avg_steps = steps
        else:
            avg_steps = (avg_steps + steps) / 2
        fails += (not converged)
        if fails >= max_fails:
            break
    if b_crit == None and last_log != None and log(avg_steps) - last_log >= 1:
        b_crit = b - step_size
    last_log = log(avg_steps)

    print("beta {:f} steps {:f} fails {:d}/{:d} log {:f}".format(b, avg_steps, fails, max_fails, log(avg_steps)))
    bvals.append(b)
    avgvals.append(avg_steps)
    if fails >= max_fails:
        break

plt.plot(bvals, avgvals)
plt.xlabel("Temperature β")
plt.ylabel("Average Markov Chain steps")
plt.title("Metropolis Glauber on {:d}x{:d} Ising model".format(dim, dim))
if b_crit != None:
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
    plt.legend(loc="upper left")
plt.savefig(output_name + ".png")