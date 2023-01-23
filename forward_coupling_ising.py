import sys
import matplotlib.pyplot as plt
import numpy as np
import random
import utils
from coupling import heat_bath_flip, metropolis_flip

if len(sys.argv) != 4:
    sys.exit("Usage: foward_coupling_ising.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

random.seed(None)

bvals = []
bsteps = []

b_crit = 0.44
bvals, bsteps = utils.simulate(Forward_Coupling(heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Heat Bath")
bvals, bsteps = utils.simulate(Forward_Coupling(metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Metropolis Filter")
plt.xlabel("Temperature β")
plt.ylabel("Steps to converge (in MC steps)")
plt.title("Ising model on {:d}x{:d} grid with foward coupling".format(dim, dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("forward_coupling{:d}x{:d}.png".format(dim, dim))
