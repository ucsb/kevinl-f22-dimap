import sys
import matplotlib.pyplot as plt
import grid_potts as gp
import random
from math import sqrt, log
import utils

if len(sys.argv) != 5:
    sys.exit("Usage: grid_potts_algs.py colors grid_dim step_size max_steps")

colors = int(sys.argv[1])
if colors > 255 or colors < 2:
    exit("Only 2-255 colors supported\n")
dim = int(sys.argv[2])
shape = (dim, dim)
step_size = float(sys.argv[3])
max_steps = int(sys.argv[4])

random.seed(None)

b_crit = log(1+sqrt(colors)) / 2
print("Critical beta for {:d} colors: {:.2f}".format(colors, b_crit))
plt.axvline(x = b_crit, color = 'r', linestyle = '-', label="Critical β")
plt.annotate(
    "β={:.2f}".format(b_crit),
    xy=(b_crit, max_steps),
    textcoords="offset points",
    xytext=(0, -15),
    ha='right',
    color="red")

bvals, bsteps = utils.simulate(gp.Magnetization(gp.heat_bath_flip, colors, shape, max_steps), step_size)
plt.plot(bvals, bsteps, color="steelblue", label="Heat bath")
plt.annotate(
    "β={:.2f}".format(bvals[-1]),
    xy=(bvals[-1], bsteps[-1]),
    textcoords="offset points",
    xytext=(0,5),
    ha='right',
    color="steelblue")

bvals, bsteps = utils.simulate(gp.Magnetization(gp.metropolis_flip, colors, shape, max_steps), step_size)
plt.plot(bvals, bsteps, color="darkorange", label="Metropolis filter")
plt.annotate(
    "β={:.2f}".format(bvals[-1]),
    xy=(bvals[-1], bsteps[-1]),
    textcoords="offset points",
    xytext=(0,5),
    ha='right',
    color="darkorange")

plt.xlim(right=(b_crit + step_size))
plt.ylim(top=max_steps)
plt.xlabel("Temperature β")
plt.ylabel("Steps to converge (in MC steps)")
plt.title("Potts model algorithms ({:d} colors, {:d}x{:d} grid)".format(colors, dim, dim))
plt.legend(loc="upper left")
plt.savefig("grid_potts_algs(q{:d}){:d}x{:d}.png".format(colors, dim, dim))