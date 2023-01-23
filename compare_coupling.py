import sys
import matplotlib.pyplot as plt
import random
from utils import simulate
from coupling_ising import Forward_Coupling, CFTP, heat_bath_flip, metropolis_flip

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
bvals, bsteps = simulate(Forward_Coupling(heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='b', label="Heat Bath (foward coupling)")
bvals, bsteps = simulate(Forward_Coupling(metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='c', label="Metropolis Filter (forward coupling)")
bvals, bsteps = simulate(CFTP(heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='g', label="Heat Bath (CFTP)")
bvals, bsteps = simulate(CFTP(metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, color='m', label="Metropolis Filter (CFTP)")
plt.xlabel("Temperature β")
plt.ylabel("Time to converge (in MC steps)")
plt.title("Comparing coupling methods for Ising model on {:d}x{:d} grid".format(dim, dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("compare_coupling{:d}x{:d}.png".format(dim, dim))
