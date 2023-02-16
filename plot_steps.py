import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    sys.exit("usage: plot_steps.py csv-file csv-file2 ...")

for i in range(1, len(sys.argv)):
    with open(sys.argv[i], 'r') as csv:
        bvals = []
        bsteps = []
        line = csv.readline()
        while len(line) > 0:
            beta, steps = line.split(',')
            bvals.append(float(beta))
            bsteps.append(float(steps))
            line = csv.readline()

        plt.plot(bvals, bsteps)
        
plt.xlabel("Temperature β")
plt.ylabel("Mixing time (in MC steps)")
title = input("Enter title for plot:\n")
plt.title(title)

print("Specific β to plot (enter nothing when done)")
while True:
    specific_beta = input()
    if len(specific_beta) == 0:
        break
    plt.axvline(x = float(specific_beta), color = 'r', linestyle = '-', label="Critical β")
plt.savefig("plot.png", facecolor='white')
