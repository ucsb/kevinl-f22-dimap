import sys
import matplotlib.pyplot as plt
import numpy as np

if len(sys.argv) < 2:
    sys.exit("Usage: plot_steps.py csv-file csv-file2 ...")

# PyPlot color cycle for annotations
cycle = plt.rcParams['axes.prop_cycle'].by_key()['color']

for i in range(1, len(sys.argv)):
    with open(sys.argv[i], 'r') as csv:
        betas = []
        steps = []
        plot_label = input(f"Label for file {i}:\n")
        for line in csv:
            row = line.split(',')
            betas.append(float(row[0]))
            steps.append(float(row[1]))

        s = np.argsort(betas)
        betas = np.array(betas)[s]
        steps = np.array(steps)[s]

        plt.plot(betas, steps, label=plot_label)

plt.xlabel("Temperature β")
plt.ylabel("Steps")
plt.title(input("Enter title for plot:\n"))

print("Specific β to plot (enter nothing when done)")
labeled_crit = False
while True:
    specific_beta = input()
    if len(specific_beta) == 0:
        break
    specific_beta = float(specific_beta)
    plt.axvline(x = specific_beta, color = 'r', linestyle = '-')

plt.legend(loc="upper left")
plt.subplots_adjust(wspace=0.3)

out_name = input("Output file name: ")
plt.savefig(out_name + ".png" if len(out_name) > 0 else "plot.png", facecolor='white')
