import sys
import matplotlib.pyplot as plt
import numpy as np
from math import exp

fields = ["Mixing time (MC steps)", "Computation time (ms)"]
plot_label = ["Mixing time", "Computation time"]

# PyPlot color cycle for annotations
cycle = plt.rcParams['axes.prop_cycle'].by_key()['color']
fig, ax = plt.subplots(1, 2, figsize=(10,5))

if len(sys.argv) != 2:
    sys.exit("Usage: benchmark.py csv-file")

fit = None

funcs = {"4/3": 4.0/3, "1/3": 1.0/3, "exp": "exp"}

function = funcs[input("Enter one in ['exp', 4/3, 1/3]: ")]

for i in range(1, len(sys.argv)):
    with open(sys.argv[i], 'r') as csv:
        sizes = []
        steps = []
        times = []

        for line in csv:
            size, step, time = line.split(',')
            sizes.append(int(size))
            steps.append(int(step))
            times.append(float(time))

        field_data = [steps, times]
        for j in range(len(fields)):
            if function == 'exp':
                fit = np.polyfit(sizes, np.log(field_data[j]), 1)
                pred = [np.exp(fit[1] + fit[0] * n) for n in sizes]
                ratio = max(field_data[j]) / max(pred)
                ax[j].plot(sizes, [p * ratio for p in pred], label=r'${:.2f} \times \exp({:f}n)$'.format(np.exp(fit[1]), fit[0]))
            else:
                base = function
                fit = np.polyfit(sizes, np.power(field_data[j], (1/base)), 1)
                pred = [(fit[1] + fit[0] * n**base) for n in sizes]
                ratio = max(field_data[j]) / max(pred)
                ax[j].plot(sizes, [p * ratio for p in pred], label=r'${:.2f} + {:.2f}n^{{{:.2f}}}$'.format(ratio*fit[1], ratio*fit[0], base))

            ax[j].ticklabel_format(style='sci', axis='y', scilimits=(0,0))
            ax[j].plot(sizes, field_data[j], label="Simulation time")
            ax[j].set_xlabel("Complete graph vertices")
            ax[j].set_ylabel(fields[j])
            ax[j].legend(loc="upper left")

beta = input("Beta: ")
beta = float(beta) if len(beta) else 0
fig.suptitle(input("Enter title for plot: ") + f" (β = {beta})")

fig.tight_layout()

out_name = input("Output file name: ")
plt.savefig(out_name + ".png" if len(out_name) > 0 else "plot.png", facecolor='white')
