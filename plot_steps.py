import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    sys.exit("Usage: plot_steps.py csv-file csv-file2 ...")

fields = []
print("Enter labels for data fields (enter nothing when done)")
while True:
    field_label = input()
    if len(field_label) == 0:
        break
    fields.append(field_label)
print("Fields:", fields)

# PyPlot color cycle for annotations
cycle = plt.rcParams['axes.prop_cycle'].by_key()['color']
fig, ax = plt.subplots(1, 2, figsize=(10,4))

for i in range(1, len(sys.argv)):
    with open(sys.argv[i], 'r') as csv:
        betas = []
        max_betas = [-1 for f in range(len(fields))]
        max_data = [-1 for f in range(len(fields))]
        field_data = [[] for f in range(len(fields))]
        plot_label = input(f"Label for file {i}: ")
        line = csv.readline()
        while len(line) > 0:
            row = line.split(',')
            beta = float(row[0])
            betas.append(beta)
            data = row[1:]
            if len(data) != len(fields):
                sys.exit(f"Error: # of data fields in {sys.argv[i]} do not match # of labels")
            for j in range(len(data)):
                datum = float(data[j])
                field_data[j].append(datum)
                if datum > max_data[j]:
                    max_data[j] = datum
                    max_betas[j] = beta
            line = csv.readline()
        for j in range(len(fields)):
            ax[j].ticklabel_format(style='sci', axis='y', scilimits=(0,0))
            if betas[-1] == max_betas[j] and max_betas[j]*1.1 > ax[j].get_xlim()[1]:
                ax[j].set_xlim(right=(max_betas[j]*1.1))
            ax[j].plot(betas, field_data[j], label=plot_label if j==0 else "")
            ax[j].set_xlabel("Temperature β")
            ax[j].set_ylabel(fields[j])
            ax[j].annotate(
                "β={:.2f}".format(max_betas[j]),
                xy=(max_betas[j], max_data[j]),
                textcoords="offset points",
                xytext=(-2,-5),
                ha='right',
                color=cycle[i-1])

fig.suptitle(input("Enter title for plot: "))

print("Specific β to plot (enter nothing when done)")
while True:
    specific_beta = input()
    if len(specific_beta) == 0:
        break
    specific_beta = float(specific_beta)
    for i in range(len(fields)):
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

out_name = input("Output file name: ")
plt.savefig(out_name + ".png" if len(out_name) > 0 else "plot.png", facecolor='white')
