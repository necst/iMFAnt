import numpy as np
import os
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rc

def geo_mean(iterable):
    a = np.array(iterable)
    return a.prod()**(1.0/len(a))

palette = ["#f7fcf0","#e0f3db","#ccebc5","#a8ddb5","#7bccc4","#4eb3d3","#2b8cbe","#08589e"]

#plt.rcParams.update({'font.sans-serif':'Helvetica','font.size':10})

folder_m = "../output_matches/"

mm = [1,2,5,10,20,50,100,0]

n = ["BRO",
     "DS9", 
     "PEN", 
     "PRO", 
     "RG1",
     "TCP"
    ]

outputlog = open("tmp.txt", "w")


merging_factors = [1,2,5,10,20,50,100,0]

input = [
    "../mfsa/bro_",
    "../mfsa/dotstar09_",
    "../mfsa/poweren_", 
    "../mfsa/protomata_",
    "../mfsa/ranges1_",
    "../mfsa/tcp_"
]

outputlog.write("dataset,time,stdev\n")

for i in input: 
    for m in merging_factors:
        t = []
        if(os.path.isfile(folder_m + i.split("/")[2] + "_multi_"+str(m)+"m_1t.output.txt")):
            file = open(folder_m + i.split("/")[2] + "_multi_"+str(m)+"m_1t.output.txt", "r") 
            tot_matches = 0
            ls = file.readlines()
            for l in ls: 
                t.append(float(l.split(",")[0]))
            tot_matches = int(ls[0].split(",")[1])
            outputlog.write(i.split("/")[2] + "_multi_"+str(m)+"m_1t.output.txt" + "," + str(np.mean(t)) +","+str(0)+ ","+str(tot_matches)+"\n")

outputlog.close()

log = open("tmp.txt", "r")

m0 = []
m1 = []
m2 = []
m5 = []
m10 = []
m20 = []
m50 = []
m100 = []

m0_th = []
m2_th = []
m5_th = []
m10_th = []
m20_th = []
m50_th = []
m100_th = []


for l in log.readlines(): 
    if ("_0m" in l): 
        m0.append(float(l.split(",")[1])/1000)
        m0_th.append(m1[len(m1)-1]/(float(l.split(",")[1])/1000))
    elif ("_1m" in l):
        m1.append(float(l.split(",")[1])/1000)
    elif ("_2m" in l):
        m2.append(float(l.split(",")[1])/1000)
        m2_th.append(m1[len(m1)-1]/(float(l.split(",")[1])/1000))
    elif ("_5m" in l):
        m5.append(float(l.split(",")[1])/1000)
        m5_th.append(m1[len(m1)-1]/(float(l.split(",")[1])/1000))
    elif ("_10m" in l):
        m10.append(float(l.split(",")[1])/1000)
        m10_th.append(m1[len(m1)-1]/(float(l.split(",")[1])/1000))
    elif ("_20m" in l):
        m20.append(float(l.split(",")[1])/1000)
        m20_th.append(m1[len(m1)-1]/(float(l.split(",")[1])/1000))
    elif ("_50m" in l):
        m50.append(float(l.split(",")[1])/1000)
        m50_th.append(m1[len(m1)-1]/(float(l.split(",")[1])/1000))
    elif ("_100m" in l):
        m100.append(float(l.split(",")[1])/1000)
        m100_th.append(m1[len(m1)-1]/(float(l.split(",")[1])/1000))

df = pd.DataFrame(columns=['dataset','M=1','M=2','M=5','M=10','M=20', 'M=50', 'M=100', 'M=all'])
# print(df)
n.append("Average")
m1.append(np.average(m1))
m2.append(np.average(m2))
m5.append(np.average(m5))
m10.append(np.average(m10))
m20.append(np.average(m20))
m50.append(np.average(m50))
m100.append(np.average(m100))
m0.append(np.average(m0))


print(n)
print(m2)

df['dataset'] = n
df['M=1'] = m1
df['M=2'] = m2
df['M=5'] = m5
df['M=10'] = m10
df['M=20'] = m20
df['M=50'] = m50
df['M=100'] = m100
df['M=all'] = m0


fig, axs = plt.subplots(1,2)

fig.set_size_inches(12,3)

print(df)

df.plot(x="dataset", y=['M=1','M=2','M=5','M=10','M=20','M=50','M=100','M=all'], ax=axs[0], legend=False,
        kind="bar", color=palette, rot=0, edgecolor='black', linewidth=0.4, width=0.8, xlabel="Datasets", ylabel="Dataset Ex. Time [s]", ylim=(0,101),
        yticks=np.arange(0, 101, 50)
        )
df.sort_values(by=['dataset'], inplace=True)

plt.tight_layout()


df.to_csv("../raw_results/iMFAnt_exec_time_singleth.csv", index=False)

df2 = pd.DataFrame(columns=['dataset','M=2','M=5','M=10','M=20', 'M=50', 'M=100', 'M=all'])

n.pop()
n.append("Geo. Mean")

m2_th.append(geo_mean(m2_th))
m5_th.append(geo_mean(m5_th))
m10_th.append(geo_mean(m10_th))
m20_th.append(geo_mean(m20_th))
m50_th.append(geo_mean(m50_th))
m100_th.append(geo_mean(m100_th))
m0_th.append(geo_mean(m0_th))


df2['dataset'] = n
df2['M=2'] = m2_th
df2['M=5'] = m5_th
df2['M=10'] = m10_th
df2['M=20'] = m20_th
df2['M=50'] = m50_th
df2['M=100'] = m100_th
df2['M=all'] = m0_th

palette = ["#e0f3db","#ccebc5","#a8ddb5","#7bccc4","#4eb3d3","#2b8cbe","#08589e"]


df2.plot(x="dataset", y=['M=2','M=5','M=10','M=20','M=50','M=100','M=all'], legend=False,
         ax=axs[1], kind="bar", color=palette, rot=0, edgecolor='black', linewidth=0.4, 
         width=0.8, xlabel="Datasets", ylabel="Throughput Imp. [x]",ylim=(0,10.1),
            yticks=np.arange(0, 15, 2)
         )
df2.sort_values(by=['dataset'], inplace=True)

df2.to_csv("../raw_results/iMFAnt_throughput_imp_singleth.csv", index=False)

fig.legend(["M=1","M=2","M=5","M=10","M=20","M=50","M=100","M=all"], loc='upper center', ncols=8, bbox_to_anchor=(0.5, 1.08))

plt.tight_layout()

plt.savefig("../plots/iMFAnt_singlethread.pdf", bbox_inches='tight')



