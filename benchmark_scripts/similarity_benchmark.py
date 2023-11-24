import Levenshtein as lv
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rc

palette = ["#f0f9e8",
"#bae4bc",
"#7bccc4",
"#43a2ca",
"#0868ac"]


inputRE = [
        "../datasets/intrusion_benchmarks/bro217_224.txt",
        "../datasets/intrusion_benchmarks/dotstar09_300.txt",
        "../datasets/PowerEN_cmp/poweren_300.txt",
        "../datasets/protomata/protomata_300.txt",
        "../datasets/intrusion_benchmarks/ranges1_300.txt",
        "../datasets/intrusion_benchmarks/tcp_300.txt",
    ]

log = "../benchmark_results/similarity_indices.csv"

o = open(log, "w")

o.write("file, distance, ratio\n")

fig = plt.figure()

plot_d = []
f_names= []

# cons. entire dataset
for file in inputRE:
    f= open(file, "r")
    lines = f.readlines()
    distances = []
    i_ratios = []
    hammings = []
    for l in lines:
        for m in lines: 
            if l != m:
                i_ratios.append(lv.ratio(l, m))
    plot_d.append(np.average(i_ratios))
    f_names.append(file.split("/")[3].split(".")[0].split("_")[0])
    # print("filen "+f_names[-1]+" ratio "+str(plot_d[-1]))
    o.write(file.split("/")[3].split(".")[0]+","+str(round(np.average(i_ratios), 4))+"\n")
    f.close()
o.close()



# cons. blocks 

# print(plot_d)

df = pd.DataFrame(columns=['dataset','ratio'])


df['dataset'] = f_names
df['ratio'] = plot_d

df.sort_values(by=['dataset'], inplace=True)
# print(df)

names = ["BRO","DS9","PEN","PRO","RG1","TCP"]
df['dataset'] = names
# print(df)


fig = df.plot.bar(x='dataset', y=['ratio'],
        rot=0,
        color=palette[2], 
        # title="Normalized INDEL similarity values for different datasets",
        fontsize=14,
        edgecolor='black', 
        linewidth=0.4, 
        width=0.8, 
        legend=False, 

        )
    
fig.set_xlabel('Datasets',fontsize=14)
fig.set_ylabel('Normalized INDEL similarity [0,1]',fontsize=14)
fig.set_xticklabels(df['dataset'],fontsize=12,rotation=90)


# plt.tight_layout()

# plt.show()

fig.get_figure().savefig('../benchmark_results/simil_indel.png', bbox_inches='tight')

# Read CSV into pandas
 
# Figure Size
 
# Horizontal Bar Plot
 
# Show Plot
# plt.show()