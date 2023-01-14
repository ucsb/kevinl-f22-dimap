import sys
from math import exp, log
import matplotlib.pyplot as plt
import numpy as np
import random

def choose_point(shape):
    h,w = shape
    x = (int)(random.uniform(0.0, 1.0) * w)
    y = (int)(random.uniform(0.0, 1.0) * h)
    return (y, x)

def flip(grid, adj_p, adj_neighbors, beta, coords, rand):
    h,w = grid.shape
    y,x = coords
    beta_p = 2.0 * beta * adj_p[y,x]
    beta_n = 2.0 * beta * (adj_neighbors - adj_p[y,x])
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    new_spin = (rand <= prob_p)
    if new_spin != grid[y,x] and new_spin == 0:
        for i in range(h):
            for j in range(w):
                if (i,j) != coords:
                    adj_p[i,j] -= 1
    grid[y,x] = new_spin

def simulate(shape, beta, max=None):
    dim = shape[0]
    steps = 0
    adj_neighbors = dim * dim - 1
    ones = np.full(shape, 1, dtype=np.int8)
    zeros = np.full(shape, 0, dtype=np.int8)
    ones_adj_p = np.full(shape, adj_neighbors, dtype=np.int16)
    zeros_adj_p = np.full(shape, 0, dtype=np.int16)
    while not np.array_equal(ones, zeros):
        point = choose_point(shape=shape)
        rand = random.uniform(0.0, 1.0)
        flip(ones, ones_adj_p, adj_neighbors, beta, coords=point, rand=rand)
        flip(zeros, zeros_adj_p, adj_neighbors, beta, coords=point, rand=rand)
        steps += 1
        if steps == max:
            break
    return steps, np.array_equal(ones, zeros)

if len(sys.argv) != 6:
    sys.exit("Usage: complete_ising.py grid_dim step_size max_iters max_fails output_name")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])
max_fails = int(sys.argv[4])
output_name = sys.argv[5]

random.seed(None)

bvals = []
avgvals = []

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
plt.title("Glauber Heat Bath on {:d}x{:d} Complete Graph".format(dim, dim))
if b_crit != None:
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
    plt.legend(loc="upper left")
plt.savefig(output_name + ".png")