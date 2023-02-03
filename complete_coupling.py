import sys
import matplotlib.pyplot as plt
import random
from utils import simulate
import complete_ising as mf

if len(sys.argv) != 4:
    sys.exit("Usage: complete_coupling.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

random.seed(None)

bvals = []
bsteps = []

b_crit = 1
bvals, bsteps = simulate(mf.Forward_Coupling(mf.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='b', label="Heat Bath (forward coupling)")
bvals, bsteps = simulate(mf.Forward_Coupling(mf.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='c', label="Metropolis Filter (forward coupling)")
bvals, bsteps = simulate(mf.CFTP(mf.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='g', label="Heat Bath (CFTP)")
bvals, bsteps = simulate(mf.CFTP(mf.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='m', label="Metropolis Filter (CFTP)")
plt.xlabel("Temperature β")
plt.ylabel("Mixing time (in MC steps)")
plt.title("Ising model coupling on {:d}x{:d} mean field".format(dim, dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("complete_coupling{:d}x{:d}.png".format(dim, dim))
