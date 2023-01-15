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

def flip(grid, grid_p, total_vertices, beta, coords, rand):
    h,w = grid.shape
    y,x = coords
    old_spin = grid[y,x]
    beta_p = 2.0 * beta * (grid_p - (old_spin == 1)) / total_vertices
    beta_n = 2.0 * beta * (total_vertices - grid_p - (old_spin == 0)) / total_vertices
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    new_spin = (rand <= prob_p)
    grid_p += (new_spin - old_spin)
    grid[y,x] = new_spin
    return grid_p

def simulate(shape, beta, max=None):
    point_rand = []
    point = choose_point(shape=shape)
    rand = random.uniform(0.0, 1.0)
    point_rand.append((point, rand))
    total_vertices = dim * dim
    while True:
        ones = np.full(shape, 1, dtype=np.int8)
        zeroes = np.full(shape, 0, dtype=np.int8)
        ones_p = dim * dim
        zeros_p = 0
        for t in range(len(point_rand) - 1, -1, -1):
            ones_p = flip(ones, ones_p, total_vertices, beta, coords=point_rand[t][0], rand=point_rand[t][1])
            zeros_p = flip(zeroes, zeros_p, total_vertices, beta, coords=point_rand[t][0], rand=point_rand[t][1])
        if len(point_rand) >= max or np.array_equal(ones, zeroes):
            return len(point_rand), np.array_equal(ones, zeroes)
        for i in range(len(point_rand)):
            point = choose_point(shape=shape)
            rand = random.uniform(0.0, 1.0)
            point_rand.append((point, rand))

if len(sys.argv) != 5:
    sys.exit("Usage: complete_ising.py grid_dim step_size max_iters max_fails")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])
max_fails = int(sys.argv[4])

random.seed(None)

bvals = []
avgvals = []

fails = 0
last_log = None
b_crit = None
for b in np.arange(0, 10, step_size):
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
plt.title("Glauber Heat Bath CFTP on {:d}x{:d} Complete Graph".format(dim, dim))
if b_crit != None:
    plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
    plt.legend(loc="upper left")
plt.savefig("complete{:d}x{:d}_cftp.png".format(dim, dim))