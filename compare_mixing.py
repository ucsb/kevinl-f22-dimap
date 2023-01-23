import sys
import matplotlib.pyplot as plt
import random
from utils import simulate
import coupling_ising as cpl
import magnetization_ising as mag

if len(sys.argv) != 4:
    sys.exit("Usage: compare_coupling.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

random.seed(None)

bvals = []
bsteps = []

b_crit = 0.44
bvals, bsteps = simulate(cpl.CFTP(cpl.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='b', label="Heat Bath (CFTP)")
bvals, bsteps = simulate(cpl.CFTP(cpl.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='c', label="Metropolis Filter (CFTP)")
bvals, bsteps = simulate(mag.Magnetization(mag.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='g', label="Heat Bath (Magnetization)")
bvals, bsteps = simulate(mag.Magnetization(mag.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='m', label="Metropolis Filter (Magnetization)")
plt.xlabel("Temperature β")
plt.ylabel("Time to converge (in MC steps)")
plt.title("Comparing mixing metrics for Ising model on {:d}x{:d} grid".format(dim, dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("compare_mixing{:d}x{:d}.png".format(dim, dim))
