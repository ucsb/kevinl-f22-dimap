import utils
import queue
import numpy as np
import random
from math import exp, log, sqrt
import matplotlib.pyplot as plt
from statistics import median
import sys

if len(sys.argv) != 3:
    exit("Usage: swendsen-wang.py dim colors")

dim = int(sys.argv[1])
shape = (dim, dim)
beta = 0
colors = int(sys.argv[2])
h, w = shape

random.seed(None)

def choose_color(colors):
    return (int)(random.uniform(0.0, 1.0) * colors)

def keep_edge(beta):
    return random.uniform(0.0, 1.0) <= (1 - exp(-1 * beta))

# print(g)

def flip(grid):
    visited = np.zeros(shape, dtype=int)
    q = queue.Queue()
    for i in range(h):
        for j in range(w):
            if visited[i,j] == 0:
                q.put((i,j))
                visited[i,j] = 1
                old_color = grid[i,j]
                new_color = choose_color(colors)
                while not q.empty():
                    y,x = q.get()
                    grid[y,x] = new_color
                    top = ((y - 1) % h, x)
                    bottom = ((y + 1) % h, x)
                    left = (y, (x - 1) % w)
                    right = (y, (x + 1) % w)
                    if visited[top] == 0 and grid[top] == old_color and keep_edge(beta):
                        visited[top] = 1
                        q.put(top)
                    if visited[bottom] == 0 and grid[bottom] == old_color and keep_edge(beta):
                        visited[bottom] = 1
                        q.put(bottom)
                    if visited[left] == 0 and grid[left] == old_color and keep_edge(beta):
                        visited[left] = 1
                        q.put(left)
                    if visited[right] == 0 and grid[right] == old_color and keep_edge(beta):
                        visited[right] = 1
                        q.put(right)

target = int(1.0/colors * shape[0] * shape[1])

def coupling(max_steps):
    grids = []
    for c in range(colors):
        grids.append(utils.Potts_Grid(shape, c, colors))
    def converged():
        for c in range(1, colors):
            if grids[0] != grids[c]:
                return False
        return True
    steps = 0
    while steps < max_steps and not converged():
        for c in range(0, colors):
            flip(grids[c])
        steps += 1
    return steps, converged()

def magnetization(max_steps):
    g = utils.Potts_Grid(shape, 0, colors)
    def converged():
        for c in g.counts:
            if abs(c - target) > colors:
                return False
        return True
    steps = 0
    while steps < max_steps and not converged():
        flip(g)
        steps += 1
    return steps

# print(coupling(10000))
# print(magnetization(10000))

step_size = 0.05
max_steps = 10000

b_crit = log(1+sqrt(colors))
print("Critical β for {:d} colors: {:.2f}".format(colors, b_crit))

bvals = []
bsteps = []
beta = 0
while True:
    trials = []
    for i in range(10):
        steps = magnetization(max_steps)
        trials.append(steps)
    med_steps = median(trials)
    if med_steps >= max_steps:
        print("beta {:f} failed to converge".format(beta))
        break
    bvals.append(beta)
    bsteps.append(med_steps)
    print("beta {:f} steps {:f}".format(beta, med_steps))
    beta += step_size

with open("swendsen-wang_q{:d}_{:d}x{:d}.csv".format(colors, dim, dim), 'w', encoding='utf-8') as f:
    for i in range(len(bvals)):
        f.write(f"{bvals[i]}, {bsteps[i]}\n")
    f.write(f"{b_crit}, 0\n{b_crit}, {bsteps[-1]}")
