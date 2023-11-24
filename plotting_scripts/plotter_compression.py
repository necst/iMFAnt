import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

palette = ["#f0f9e8",
    "#ccebc5",
    "#a8ddb5",
    "#7bccc4",
    "#4eb3d3",
    "#2b8cbe",
    "#08589e"]

#plt.rcParams.update({'font.sans-serif':'Helvetica','font.size':10})

df1 = pd.read_csv("../raw_results/states_comp.csv")

df2 = pd.read_csv("../raw_results/transitions_comp.csv")

fig, axs = plt.subplots(1,2)

fig.set_size_inches(12,4)

df1.sort_values(by=['dataset'], inplace=True)

df1['dataset']=["BRO","DS9","PEN","PRO","RG1","TCP"]

df1.plot(x="dataset", y=['M=2','M=5','M=10','M=20','M=50','M=100','M=all'], ax=axs[0], legend=False,
        kind="bar", color=palette, rot=0, edgecolor='black', linewidth=0.4, width=0.8, 
        xlabel="Datasets", ylabel="State Comp. [%]"
        )

print("top states compression (avg): "+str(np.average(df1['M=all'])))

plt.tight_layout()

df2['dataset']=["BRO","DS9","PEN","PRO","RG1","TCP"]



df2.plot(x="dataset", y=['M=2','M=5','M=10','M=20','M=50','M=100','M=all'], legend=False,
         ax=axs[1], kind="bar", color=palette, rot=0, edgecolor='black', linewidth=0.4, 
         width=0.8, xlabel="Datasets", ylabel="Trans. Comp. [%]"
         )

print("top transitions compression (avg): "+str(np.average(df2['M=all'])))

fig.legend(["M=2","M=5","M=10","M=20","M=50","M=100","M=all"], loc='upper center', ncols=7, bbox_to_anchor=(0.5, 1.1))

plt.tight_layout()

plt.savefig("../plots/compression.pdf", bbox_inches='tight')
