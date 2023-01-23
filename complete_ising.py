import utils
import random
import numpy as np
from math import exp

# Thanks to Rose and Luke for help with complete graph
def heat_bath_flip(grid, grid_p, grid_n, beta, coords, rand, spin):
    h,w = grid.shape
    y,x = coords
    old_spin = grid[y,x]
    beta_p = beta * (grid_p - (old_spin == 1))
    beta_n = beta * (grid_n - (old_spin == 0))
    prob_p = exp(beta_p) / (exp(beta_n) + exp(beta_p))
    new_spin = (rand <= prob_p)
    grid_p += (new_spin - old_spin)
    grid[y,x] = new_spin
    return grid_p

class Forward_Coupling:
    def __init__(self, flip, shape, max_iters):
        self.flip = flip
        self.shape = shape
        self.max_iters = max_iters
    def run(self, beta):
        steps = 0
        ones = np.full(shape, 1, dtype=np.int8)
        zeros = np.full(shape, 0, dtype=np.int8)
        total_vertices = self.shape[0] * self.shape[1]
        ones_p = total_vertices
        zeros_p = 0
        while not np.array_equal(ones, zeros):
            point = utils.choose_point(self.shape)
            rand = random.uniform(0.0, 1.0)
            spin = utils.choose_spin(0.5)
            ones_p = self.flip(ones, ones_p, total_vertices, beta, point, rand)
            zeros_p = self.flip(zeros, zeros_p, total_vertices, beta, point, rand)
            steps += 1
            if steps == self.max_iters:
                break
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
        total_vertices = self.shape[0] * self.shape[1]
        beta = 2 * beta / (total_vertices - 1)
        while True:
            ones = np.full(self.shape, 1, dtype=np.int8)
            zeros = np.full(self.shape, 0, dtype=np.int8)
            ones_p = total_vertices
            zeros_p = 0
            for t in range(len(point_rand_spin) - 1, -1, -1):
                ones_p = self.flip(ones, ones_p, total_vertices - ones_p, beta, point_rand_spin[t][0], point_rand_spin[t][1], point_rand_spin[t][2])
                zeros_p = self.flip(zeros, zeros_p, total_vertices - zeros_p, beta, point_rand_spin[t][0], point_rand_spin[t][1], point_rand_spin[t][2])
            if len(point_rand_spin) >= self.max_iters or np.array_equal(ones, zeros):
                return len(point_rand_spin), np.array_equal(ones, zeros)
            for i in range(len(point_rand_spin)):
                point = utils.choose_point(self.shape)
                rand = random.uniform(0.0, 1.0)
                spin = utils.choose_spin(0.5)
                point_rand_spin.append((point, rand, spin))


# def simulate(shape, beta, max=None):
#     dim = shape[0]
#     steps = 0
#     ones = np.full(shape, 1, dtype=np.int8)
#     zeros = np.full(shape, 0, dtype=np.int8)
#     total_vertices = dim * dim
#     ones_p = dim * dim
#     zeros_p = 0
#     while not np.array_equal(ones, zeros):
#         point = choose_point(shape=shape)
#         rand = random.uniform(0.0, 1.0)
#         ones_p = flip(ones, ones_p, total_vertices, beta, coords=point, rand=rand)
#         zeros_p = flip(zeros, zeros_p, total_vertices, beta, coords=point, rand=rand)
#         steps += 1
#         if steps == max:
#             break
#     return steps, np.array_equal(ones, zeros)

# if len(sys.argv) != 5:
#     sys.exit("Usage: complete_ising.py grid_dim step_size max_iters max_fails")

# dim = int(sys.argv[1])
# shape = (dim, dim)
# step_size = float(sys.argv[2])
# max_iters = int(sys.argv[3])
# max_fails = int(sys.argv[4])

# random.seed(None)

# bvals = []
# avgvals = []

# fails = 0
# last_log = None
# b_crit = None
# for b in np.arange(0, 10, step_size):
#     avg_steps = 0
#     for i in range(10):
#         steps, converged = simulate(shape, b, max=max_iters)
#         if i == 0:
#             avg_steps = steps
#         else:
#             avg_steps = (avg_steps + steps) / 2
#         fails += (not converged)
#         if fails >= max_fails:
#             break
#     if b_crit == None and last_log != None and log(avg_steps) - last_log >= 1:
#         b_crit = b - step_size
#     last_log = log(avg_steps)

#     print("beta {:f} steps {:f} fails {:d}/{:d} log {:f}".format(b, avg_steps, fails, max_fails, log(avg_steps)))
#     bvals.append(b)
#     avgvals.append(avg_steps)
#     if fails >= max_fails:
#         break

# plt.plot(bvals, avgvals)
# plt.xlabel("Temperature β")
# plt.ylabel("Average Markov Chain steps")
# plt.title("Glauber Heat Bath on {:d}x{:d} Complete Graph".format(dim, dim))
# if b_crit != None:
#     plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
#     plt.legend(loc="upper left")
# plt.savefig("complete{:d}x{:d}.png".format(dim, dim))