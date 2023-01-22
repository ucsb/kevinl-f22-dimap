import random
import matplotlib.pyplot as plt
import numpy as np
from statistics import median

def simulate(mixer, step_size):
    bvals = []
    bsteps = []
    b = 0
    while True:
        trials = []
        for i in range(10):
            steps, converged = mixer.run(b)
            if converged:
                trials.append(steps)
            else:
                print("beta {:f} failed to converge".format(b))
                return bvals, bsteps
        if len(trials) > 0:
            med_steps = median(trials)
            bvals.append(b)
            bsteps.append(med_steps)
            print("beta {:f} steps {:f}".format(b, med_steps))
            b += step_size
    return bvals, bsteps

def choose_point(shape):
    h,w = shape
    x = (int)(random.uniform(0.0, 1.0) * w)
    y = (int)(random.uniform(0.0, 1.0) * h)
    return (y, x)

def choose_spin(prob_p):
    return (random.uniform(0.0, 1.0) <= prob_p)

def shuffle_grid(grid):
    h,w = grid.shape
    for i in range(w):
        for j in range(h):
            grid[j, i] = (int)(random.uniform(0.0, 1.0) <= 0.5)

def side_by_side(grid1, grid2):
    if grid1.shape != grid2.shape:
        exit("Printing spins with incompatible transition matrix")
    for i in range(len(grid1)):
        print(grid1[i], grid2[i])

def sum_neighbors(grid, coords):
    h,w = grid.shape
    y,x = coords
    sums = [0, 0]
    sums[grid[(y - 1) % h, x]] += 1
    sums[grid[(y + 1) % h, x]] += 1
    sums[grid[y, (x - 1) % w]] += 1
    sums[grid[y, (x + 1) % w]] += 1
    return sums
