import sys
import matplotlib.pyplot as plt
import random
from utils import simulate
import grid_ising as grid

if len(sys.argv) != 4:
    sys.exit("Usage: grid_coupling.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

random.seed(None)

bvals = []
bsteps = []

b_crit = 0.44
bvals, bsteps = simulate(grid.Forward_Coupling(grid.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='b', label="Heat Bath (forward coupling)")
bvals, bsteps = simulate(grid.Forward_Coupling(grid.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='c', label="Metropolis Filter (forward coupling)")
bvals, bsteps = simulate(grid.CFTP(grid.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='g', label="Heat Bath (CFTP)")
bvals, bsteps = simulate(grid.CFTP(grid.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='m', label="Metropolis Filter (CFTP)")
plt.xlabel("Temperature β")
plt.ylabel("Mixing time (in MC steps)")
plt.title("Ising model coupling on {:d}x{:d} grid".format(dim, dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("grid_coupling{:d}x{:d}.png".format(dim, dim))
