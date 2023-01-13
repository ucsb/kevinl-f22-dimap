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

def choose_spin(prob_pos):
    return (random.uniform(0.0, 1.0) <= prob_pos)

def flip(grid, mchrome, beta, coords, new_spin, rand):
    h,w = grid.shape
    y,x = coords
    old_val = grid[y,x]

    # Calculate monochromatic edges at vertex of interest
    mchrome_before = 0
    mchrome_before += (grid[y,x] == grid[(y - 1) % h, x])
    mchrome_before += (grid[y,x] == grid[(y + 1) % h, x])
    mchrome_before += (grid[y,x] == grid[y, (x - 1) % w])
    mchrome_before += (grid[y,x] == grid[y, (x + 1) % w])

    # Set new spin and recalculate monochromatic edges
    grid[y,x] = new_spin
    mchrome_after = 0
    mchrome_after += (grid[y,x] == grid[(y - 1) % h, x])
    mchrome_after += (grid[y,x] == grid[(y + 1) % h, x])
    mchrome_after += (grid[y,x] == grid[y, (x - 1) % w])
    mchrome_after += (grid[y,x] == grid[y, (x + 1) % w])
    # print("mchrome_after {:d} mchrome_before {:d} diff {:d}".format(mchrome_after, mchrome_before, mchrome_after - mchrome_before))

    # Calculate acceptance probability
    prob_accept = min(1, exp(2 * (mchrome_after - mchrome_before) * beta))

    # Set new mchrone count if below acceptance threshold. Else, reset grid
    if rand <= prob_accept:
        return mchrome + mchrome_after - mchrome_before
    else:
        grid[y,x] = old_val
        return mchrome
    

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
    mchrome_ones = 2 * dim * dim
    mchrome_zeros = 2 * dim * dim
    while not np.array_equal(ones, zeros):
        point = choose_point(shape=shape)
        spin = choose_spin(0.5)
        rand = random.uniform(0.0, 1.0)
        mchrome_ones = flip(ones, mchrome_ones, beta, point, spin, rand)
        mchrome_zeros = flip(zeros, mchrome_zeros, beta, point, spin, rand)
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

fails = 0
b_crit = None
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
    print("beta {:f} steps {:f} fails {:d}/{:d}".format(b, avg, fails, max_fails))
    if b_crit == None and len(avgvals) > 0 and avg / avgvals[-1] >= 4:
        b_crit = b - 0.02
    bvals.append(b)
    avgvals.append(avg)
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