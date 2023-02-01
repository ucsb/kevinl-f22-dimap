import sys
import random
import utils
import grid_ising as grid
import matplotlib.pyplot as plt
from math import log, sqrt

random.seed(None)

if len(sys.argv) != 4:
    sys.exit("Usage: grid_cftp.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

b_crit = log(1+sqrt(2)) / 2
plt.xlim(right=(b_crit + step_size))
plt.ylim(top=max_iters)
bvals, bsteps = utils.simulate(grid.CFTP(grid.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color="darkorange", label="Heat bath")
plt.annotate(
    "β={:.2f}".format(bvals[-1]),
    xy=(bvals[-1], bsteps[-1]),
    textcoords="offset points",
    xytext=(0,5),
    ha='right',
    color="darkorange")
bvals, bsteps = utils.simulate(grid.CFTP(grid.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color = "steelblue", label="Metropolis filter")
plt.annotate(
    "β={:.2f}".format(bvals[-1]),
    xy=(bvals[-1], bsteps[-1]),
    textcoords="offset points",
    xytext=(0,15),
    ha='right',
    color="steelblue")
plt.xlabel("Temperature β")
plt.ylabel("Mixing time (in MC steps)")
plt.title("{:d}x{:d} grid Ising CFTP".format(dim,dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.annotate(
    "β={:.2f}".format(b_crit),
    xy=(b_crit, max_iters),
    textcoords="offset points",
    xytext=(0, -15),
    ha='right',
    color="red")
plt.legend(loc="upper left")
plt.savefig("grid_cftp{:d}x{:d}.png".format(dim,dim))
