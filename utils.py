import random
import matplotlib.pyplot as plt
import numpy as np
from statistics import median
from math import exp

class Grid:
    def __init__(self, shape, value, p_vertices=None):
        self.grid = np.full(shape, value, dtype=np.int8)
        self.shape = shape
        self.total_vertices = shape[0] * shape[1]
        if p_vertices == None:
            self.p_vertices = np.sum(self.grid)
        else:
            self.p_vertices = p_vertices
    def __eq__(self, other):
        if isinstance(other, Grid):
            return np.array_equal(self.grid,other.grid)
        return False
    def __neq__(self, other):
        return not (self == other)
    def __getitem__(self, coords):
        y,x = coords
        return self.grid[y,x]
    def __setitem__(self, coords, new_spin):
        y,x = coords
        self.p_vertices += (new_spin - self.grid[y,x])
        self.grid[y,x] = new_spin
    def __str__(self):
        return self.grid.__str__() + "\npositive {:d} negative {:d}".format(self.p(), self.n())
    def p(self):
        return self.p_vertices
    def n(self):
        return self.total_vertices - self.p_vertices

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

def coupling_params(shape):
    return (choose_point(shape), random.uniform(0.0, 1.0), choose_spin(0.5))