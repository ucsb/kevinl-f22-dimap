import sys
import random
import utils
import grid_ising as grid
import matplotlib.pyplot as plt

random.seed(None)

if len(sys.argv) != 4:
    sys.exit("Usage: grid_cftp.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

bvals = []
bsteps = []
b_crit =  .44

bvals, bsteps = utils.simulate(grid.CFTP(grid.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Heat bath")
bvals, bsteps = utils.simulate(grid.CFTP(grid.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Metropolis filter")
plt.xlabel("Temperature β")
plt.ylabel("Mixing time (in MC steps)")
plt.title("{:d}x{:d} grid Ising CFTP".format(dim,dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("grid_cftp{:d}x{:d}.png".format(dim,dim))
