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
size = dim * dim
shape = (dim, dim)
beta = None
colors = int(sys.argv[2])
h, w = shape

random.seed(None)

def choose_color(colors):
    return (int)(random.uniform(0.0, 1.0) * colors)

def keep_edge(beta):
    return random.uniform(0.0, 1.0) > exp(-1 * beta)

# print(g)

def coupling_flip(grid, rand_colors, edge_choices):
    visited = np.zeros(shape, dtype=int)
    q = queue.Queue()
    # print(edge_choices)
    # print(grid)
    for i in range(h):
        for j in range(w):
            if visited[i,j] == 0:
                q.put((i,j))
                visited[i,j] = 1
                old_color = grid[i,j]
                new_color = rand_colors[i,j]
                while not q.empty():
                    y,x = q.get()
                    grid[y,x] = new_color
                    # print(grid)
                    top = ((y - 1) % h, x)
                    bottom = ((y + 1) % h, x)
                    left = (y, (x - 1) % w)
                    right = (y, (x + 1) % w)
                    if visited[top] == 0 and grid[top] == old_color and edge_choices[top]:
                        visited[top] = 1
                        q.put(top)
                    if visited[bottom] == 0 and grid[bottom] == old_color and edge_choices[bottom]:
                        visited[bottom] = 1
                        q.put(bottom)
                    if visited[left] == 0 and grid[left] == old_color and edge_choices[left]:
                        visited[left] = 1
                        q.put(left)
                    if visited[right] == 0 and grid[right] == old_color and edge_choices[right]:
                        visited[right] = 1
                        q.put(right)
    # print(grid)

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
    halves = utils.Potts_Grid(shape, 0, colors)
    for i in range(dim):
        for j in range(dim):
            halves[i,j] = ((i * dim) + j >= size / 2)
    chess = utils.Potts_Grid(shape, 1, colors)
    for i in range(dim):
        for j in range(dim):
            chess[i,j] = (i + j) % colors
    steps = 0
    while steps < max_steps and halves != chess:
        rand_colors = np.array([[choose_color(colors) for i in range(dim)] for j in range(dim)])
        edge_choices = np.array([[keep_edge(beta) for i in range(dim)] for j in range(dim)])
        coupling_flip(halves, rand_colors, edge_choices)
        coupling_flip(chess, rand_colors, edge_choices)
        steps += 1
    return steps

def magnetization(max_steps):
    zeros = utils.Potts_Grid(shape, 0, colors)
    ones = utils.Potts_Grid(shape, 1, colors)
    chess = utils.Potts_Grid(shape, 0, colors)
    for i in range(dim):
        for j in range(dim):
            chess[i,j] = (i + j) % colors
    grids = [zeros, ones, chess]
    def converged(grid1, grid2):
        for i in range(colors):
            if abs(grid1.counts[i] - grid2.counts[i]) > colors:
                return False
        return True
    steps = 0
    while steps < max_steps and not converged(zeros, ones) and not converged(ones, chess) and not converged(zeros, chess):
        flip(zeros)
        flip(ones)
        flip(chess)
        steps += 1
    return steps

max_steps = 500

# Ullrich: https://arxiv.org/pdf/1212.4908.pdf
b_crit = log(1+sqrt(colors))
print("Critical β for {:d} colors: {:.2f}".format(colors, b_crit))

beta = .8
step_size = 0.01
bvals = []
bsteps = []
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
