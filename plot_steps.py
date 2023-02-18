import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 3:
    sys.exit("usage: plot_steps.py fields csv-file csv-file2 ...")

# PyPlot color cycle for annotations
cycle = plt.rcParams['axes.prop_cycle'].by_key()['color']

fields = int(sys.argv[1])
field_labels = []
for i in range(fields):
    field_labels.append(input(f"Label for field {i+1}: "))

fig, ax = plt.subplots(1, 2, figsize=(10,4))

for i in range(2, len(sys.argv)):
    plot_label = input(f"Label for file {i-1}: ")
    fieldlists = [[] for f in range(fields)]
    with open(sys.argv[i], 'r') as csv:
        bvals = []
        line = csv.readline()
        while len(line) > 0:
            row = line.split(',')
            bvals.append(float(row[0]))
            fieldvals = row[1:]
            for j in range(len(fieldvals)):
                fieldlists[j].append(float(fieldvals[j]))
            line = csv.readline()
        for j in range(fields):
            ax[j].set_xlim(right=(bvals[-1]*1.1))
            ax[j].plot(bvals, fieldlists[j], label=plot_label if j==0 else "")
            ax[j].set_xlabel("Temperature β")
            ax[j].set_ylabel(field_labels[j])
            ax[j].annotate(
                "β={:.2f}".format(bvals[-1]),
                xy=(bvals[-1], fieldlists[j][-1]),
                textcoords="offset points",
                xytext=(-2,-5),
                ha='right',
                color=cycle[i-2])

fig.suptitle(input("Enter title for plot: "))

print("Specific β to plot (enter nothing when done)")
while True:
    specific_beta = input()
    if len(specific_beta) == 0:
        break
    specific_beta = float(specific_beta)
    for i in range(fields):
        ax[i].axvline(x = specific_beta, color = 'r', linestyle = '-', label="Critical β" if i==0 else "")
        ax[i].annotate(
            "β={:.2f}".format(specific_beta),
            xy=(specific_beta, 0),
            textcoords="offset points",
            xytext=(-2,-2),
            ha='right',
            color="red")

fig.legend(loc="upper left")
plt.subplots_adjust(wspace=0.3)
plt.savefig("plot.png", facecolor='white')
