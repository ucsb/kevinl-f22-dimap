import utils
from math import exp
import random
import numpy as np

def heat_bath_flip(grid, beta, coords, rand, new_spin):
    sums = utils.sum_neighbors(grid, coords)
    beta_p = 2.0 * beta * sums[1]
    beta_n = 2.0 * beta * sums[0]
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    grid[coords[0], coords[1]] = (rand <= prob_p)

def metropolis_flip(grid, beta, coords, rand, new_spin):
    h,w = grid.shape
    y,x = coords
    sums = utils.sum_neighbors(grid, coords)
    # Index into neighbor sums based on spin
    mchrome_before = sums[grid[y,x]]
    mchrome_after = sums[new_spin]
    # Calculate acceptance probability
    prob_accept = min(1, exp(2 * (mchrome_after - mchrome_before) * beta))
    # Accept new state with calculated probability
    if rand <= prob_accept:
        grid[y,x] = new_spin

class Forward_Coupling:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
    def run(self, beta):
        steps = 0
        ones = np.full(self.shape, 1, dtype=np.int8)
        zeros = np.full(self.shape, 0, dtype=np.int8)
        while steps < self.max_iters and not np.array_equal(ones, zeros):
            point = utils.choose_point(self.shape)
            rand = random.uniform(0.0, 1.0)
            spin = utils.choose_spin(0.5)
            self.flip(ones, beta, point, rand, spin)
            self.flip(zeros, beta, point, rand, spin)
            steps += 1
        return steps, np.array_equal(ones, zeros)

class CFTP:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
    def run(self, beta):
        point_rand_spin = []
        point = utils.choose_point(self.shape)
        rand = random.uniform(0.0, 1.0)
        spin = utils.choose_spin(0.5)
        point_rand_spin.append((point, rand, spin))
        while True:
            ones = np.full(self.shape, 1, dtype=np.int8)
            zeroes = np.full(self.shape, 0, dtype=np.int8)
            for t in range(len(point_rand_spin) - 1, -1, -1):
                self.flip(ones, beta, point_rand_spin[t][0], point_rand_spin[t][1], point_rand_spin[t][2])
                self.flip(zeroes, beta, point_rand_spin[t][0], point_rand_spin[t][1], point_rand_spin[t][2])
            if len(point_rand_spin) >= self.max_iters or np.array_equal(ones, zeroes):
                return len(point_rand_spin), np.array_equal(ones, zeroes)
            for i in range(len(point_rand_spin)):
                point = utils.choose_point(self.shape)
                rand = random.uniform(0.0, 1.0)
                spin = utils.choose_spin(0.5)
                point_rand_spin.append((point, rand, spin))
