import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    sys.exit("usage: executable csv-file-name")

csv = open(sys.argv[1], 'r').read()
lines = csv.split('\n')

beta_list = []
avg_it_list = []

for i in range(0, len(lines)):
    # print(lines[i].split(','))
    if len(lines[i]) > 0:
        beta, avg_it = lines[i].split(',')
        beta_list.append(float(beta))
        avg_it_list.append(int(avg_it))

plt.plot(beta_list, avg_it_list)
plt.xlabel("Temperature beta")
plt.ylabel("Average Markov Chain steps")
plt.title("Ising model for temperature beta")
# plt.xlim(left=0, right=25)
plt.savefig("ising_graph.png", facecolor='white')
plt.show()