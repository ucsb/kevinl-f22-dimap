import sys
import random
import utils
import complete_ising as meanf
import matplotlib.pyplot as plt

random.seed(None)

if len(sys.argv) != 4:
    sys.exit("Usage: complete_cftp.py grid_dim step_size max_iters")

dim = int(sys.argv[1])
shape = (dim, dim)
step_size = float(sys.argv[2])
max_iters = int(sys.argv[3])

bvals = []
bsteps = []
b_crit =  1

bvals, bsteps = utils.simulate(meanf.CFTP(meanf.heat_bath_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Heat bath")
bvals, bsteps = utils.simulate(meanf.CFTP(meanf.metropolis_flip, shape, max_iters), step_size)
plt.plot(bvals, bsteps, label="Metropolis filter")
plt.xlabel("Temperature β")
plt.ylabel("Mixing time (in MC steps)")
plt.title("{:d}x{:d} mean field Ising CFTP".format(dim,dim))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.legend(loc="upper left")
plt.savefig("complete_cftp{:d}x{:d}.png".format(dim,dim))
