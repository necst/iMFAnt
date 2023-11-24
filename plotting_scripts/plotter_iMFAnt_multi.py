import matplotlib.pyplot as plt
import numpy as np
import os
import pandas as pd
from matplotlib.ticker import ScalarFormatter
import matplotlib.ticker as ticker
import matplotlib.lines as mlines
from matplotlib.legend_handler import HandlerBase

class HandlerLine2D(HandlerBase):
    def create_artists(self, legend, orig_handle, xdescent, ydescent, width, height, fontsize, trans):
        line = mlines.Line2D([xdescent + width/2], [ydescent + height/2],
                              lw=orig_handle.get_linewidth(), color=orig_handle.get_color())
        return [line]

def myLogFormat(y,pos):
    decimalplaces = int(np.maximum(-np.log10(y),0))     # =0 for numbers >=1
    formatstring = '{{:.{:1d}f}}'.format(decimalplaces)
    return formatstring.format(y)

# plt.rcParams.update({'font.sans-serif':'Helvetica','font.size':10})

palette = ["#e0f3db","#ccebc5","#a8ddb5","#7bccc4","#4eb3d3","#2b8cbe","#08589e"]

markers = ['o','s','^','D','v', 'p','P']

folder_m = "../output_matches/"

num_threads = [1,2,4,8,16,32,64,128]

merging_factors = [1,5,10,20,50,100,0]

input = [
    "../mfsa/bro_",
    "../mfsa/dotstar09_",
    "../mfsa/poweren_", 
    "../mfsa/protomata_",
    "../mfsa/ranges1_",
    "../mfsa/tcp_"
]

outputlog = open("tmp.txt", "w")

outputlog.write("dataset,time,stdev\n")

abs_min = []
abs_min_t = []
abs_min_m = []

min_one = []
min_t_one = []

best_t_ut = []
best_t_ut_t = []
best_t_ut_m = []

for i in input: 
    abs_min_v = 1000000000
    abs_min_t_v = 0
    abs_min_m_v = 0
    min_one_v = 1000000000
    min_t_one_v = 0
    for m in merging_factors:
        for n in num_threads:
            t = []
            print(folder_m + i.split("/")[2] + "_multi_"+str(m)+"m_"+str(n)+"t.output.txt")
            if(os.path.isfile(folder_m + i.split("/")[2] + "_multi_"+str(m)+"m_"+str(n)+"t.output.txt")):
                file = open(folder_m + i.split("/")[2] + "_multi_"+str(m)+"m_"+str(n)+"t.output.txt", "r") 
                tot_matches = 0
                ls = file.readlines()
                for l in ls: 
                    t.append(float(l.split(",")[0]))
                tot_matches = int(ls[0].split(",")[1])
                outputlog.write(i.split("/")[2] + "_multi_"+str(m)+"m_"+str(n)+"t.output.txt" + "," + str(np.mean(t)) +","+str(0)+ ","+str(tot_matches)+"\n")
            if m == 1 and (np.mean(t)<min_one_v):
                    min_one_v = np.mean(t)
                    min_t_one_v = num_threads.index(n)
            elif(m!=1 and np.mean(t)<abs_min_v):
                abs_min_v = np.mean(t)
                abs_min_t_v = num_threads.index(n)
                abs_min_m_v = merging_factors.index(m)
    abs_min.append(abs_min_v/1000)
    abs_min_t.append(abs_min_t_v)
    abs_min_m.append(abs_min_m_v)
    min_one.append(min_one_v/1000)
    min_t_one.append(min_t_one_v)


for i in input: 
    print("\n\ndataset: "+i)
    best_t_ut_v = min_one[input.index(i)]
    best_t_ut_t_v = 6
    best_t_ut_m_v = 0
    found = False
    for n in num_threads:
    
        # print("\nmerging factor: "+str(m))
        if not found:
            for m in merging_factors:
                t = []
                if(os.path.isfile(folder_m + i.split("/")[2] + "_multi_"+str(m)+"m_"+str(n)+"t.output.txt")):
                    file = open(folder_m + i.split("/")[2] + "_multi_"+str(m)+"m_"+str(n)+"t.output.txt", "r") 
                    ls = file.readlines()
                    for l in ls: 
                        t.append(float(l.split(",")[0]))
                if m != 1 and ((best_t_ut_v == min_one[input.index(i)] and (np.mean(t)/1000)<=min_one[input.index(i)] and n <= num_threads[best_t_ut_t_v]) or
                               ((np.mean(t)/1000)<=best_t_ut_v and n <= num_threads[best_t_ut_t_v])
                               ):
                        best_t_ut_v = np.mean(t)/1000
                        best_t_ut_t_v = num_threads.index(n)
                        best_t_ut_m_v = merging_factors.index(m)
    best_t_ut.append(best_t_ut_v)
    best_t_ut_t.append(best_t_ut_t_v)
    best_t_ut_m.append(best_t_ut_m_v)

outputlog.close()

outputlog = open("tmp.txt", "r")

fig, axs = plt.subplots(1,6)
fig.set_size_inches(12,2)

for ax in axs.flat:
    ax.set(xlabel='x-label')

names =["1","2","4","8","16","32","64","128"]

ls = outputlog.readlines()

datasets = [
    "BRO","DS9","PEN","PRO","RG1","TCP"
    ]

idx=0

plotted = []
r=0
c=0

for l in range(1, len(ls), len(names)*len(merging_factors)):
    t1 = []
    t2 = []
    t4 = []
    t8 = []
    t16 = []
    t32 = []
    t64 = []
    t128 = []
    for ll in range(len(merging_factors)): 
        t1.append(float(ls[l+ll*len(names)].split(",")[1])/1000)
        t2.append(float(ls[l+ll*len(names)+1].split(",")[1])/1000)
        t4.append(float(ls[l+ll*len(names)+2].split(",")[1])/1000)
        t8.append(float(ls[l+ll*len(names)+3].split(",")[1])/1000)
        t16.append(float(ls[l+ll*len(names)+4].split(",")[1])/1000)
        t32.append(float(ls[l+ll*len(names)+5].split(",")[1])/1000)
        t64.append(float(ls[l+ll*len(names)+6].split(",")[1])/1000)
        t128.append(float(ls[l+ll*len(names)+7].split(",")[1])/1000)

    df = pd.DataFrame({'1':t1,'2':t2, '4':t4, '8':t8, '16':t16, '32':t32, '64':t64, '128':t128})

    df1 = df.T

    df1.columns = ["M=1","M=5","M=10","M=20","M=50","M=100","M=all"]

    df1["T"]=names
    df1['M=1'] = df1['M=1'].astype(float)
    df1['M=5'] = df1['M=5'].astype(float)
    df1['M=10'] = df1['M=10'].astype(float)
    df1['M=20'] = df1['M=20'].astype(float)
    df1['M=50'] = df1['M=50'].astype(float)
    df1['M=100'] = df1['M=100'].astype(float)
    df1['M=all'] = df1['M=all'].astype(float)
    df1['T']=df1['T'].astype(int)

    df1.drop(columns=["T"])

    flag = True

    col_idx = 0
    col_idx_used = []

    for i, column in enumerate(df1.columns):


        # print("r:"+str(r)+", c:"+str(c))

        ax = axs[idx]

        ax.set_yscale('log')
        
        plotted = []

        if column != "T":
            p = ax.plot(df1[column],linestyle="solid", color=palette[col_idx % len(palette)],linewidth = 1, label='_nolegend_')
            scatter=ax.scatter(range(0,8), df1[column], marker = markers[col_idx%len(markers)], color=palette[col_idx % len(palette)], edgecolor='black', linewidths=0.5,s=20, label=column)
            plotted.append(scatter)
            scatter.set_zorder(10)
            ax.set_title(datasets[idx])
            ax.set_xticklabels(names, rotation=70)
            ax.set_xticks(range(0,8,1))
            ax.set_xlabel("#Threads")
            if(idx==0):# or idx == 3):
                ax.set_ylabel("Time [s] - log. scale")

        ax = axs[idx]
        ax.set_yscale('log')
        col_idx_used.append(col_idx)
                
        col_idx = col_idx+1 


    ax.yaxis.set_major_locator(ticker.LogLocator(base=10.0, numticks=5))
    if(idx==0):
        ax.yaxis.set_minor_locator(ticker.LogLocator(base=10.0, subs=([0.2,0.4]), numticks=5))
    else:
        ax.yaxis.set_minor_locator(ticker.LogLocator(base=10.0, subs=([0.25,0.5]), numticks=5))
    ax.yaxis.set_minor_formatter(ticker.FuncFormatter(myLogFormat))
    ax.yaxis.set_major_formatter(ticker.FuncFormatter(myLogFormat))

    ax.scatter(min_t_one[idx],min_one[idx], s=60, color=palette[0], edgecolors='black',linewidth=1,
    marker = 'o', zorder=10, label="Top Perf. M=1")

    ax.scatter(best_t_ut_t[idx],best_t_ut[idx], s=70, color=palette[best_t_ut_m[idx]%len(palette)], label='_nolegend_',
    marker = markers[best_t_ut_m[idx]%len(markers)], zorder=10)

    ax.scatter(abs_min_t[idx],abs_min[idx], s=70, color=palette[best_t_ut_m[idx]%len(palette)], label='_nolegend_',
    marker = markers[abs_min_m[idx]%len(markers)], zorder=10)

    ax.scatter(best_t_ut_t[idx],best_t_ut[idx], s=70, color='none', edgecolors="blue",linewidth=1.5,
    marker = markers[best_t_ut_m[idx]%len(markers)], zorder=10, label = "Best Th. Ut.")

    ax.scatter(abs_min_t[idx],abs_min[idx], s=70, color='none', edgecolors="#d7191c",linewidth=1.5,
    marker = markers[abs_min_m[idx]%len(markers)], zorder=10, label = "Best Perf. M>1")

    df1.to_csv("../raw_results/"+datasets[idx]+"_multi.csv", index=False)

    df1.drop(df1.index , inplace=True)

    idx = idx+1
    

plt.subplots_adjust(hspace=0.6,
                    wspace=0.3)

r=0
c=0


legend_handles = [
    plt.Line2D([], [], marker='o', color=palette[0], markeredgecolor='black', markeredgewidth=0.5, markersize=7, label='M=1'),
    plt.Line2D([], [], marker='s', color=palette[1],  markeredgecolor='black', markeredgewidth=0.5, markersize=7, label='M=5'),
    plt.Line2D([], [], marker='^', color=palette[2], markeredgecolor='black', markeredgewidth=0.5, markersize=7, label='M=10'),
    plt.Line2D([], [], marker='D', color=palette[3], markeredgecolor='black', markeredgewidth=0.5, markersize=7, label='M=20'),
    plt.Line2D([], [], marker='v', color=palette[4], markeredgecolor='black', markeredgewidth=0.5, markersize=7, label='M=50'),
    plt.Line2D([], [], marker='p', color=palette[5],markeredgecolor='black', markeredgewidth=0.5, markersize=7, label='M=100'),
    plt.Line2D([], [], marker='P', color=palette[6], markeredgecolor='black', markeredgewidth=0.5, markersize=7, label='M=all'),
    plt.Line2D([], [], marker='o', color=palette[0], markeredgecolor='black', markeredgewidth=2, markersize=10, label='Best Perf. M=1'),
    plt.Line2D([], [], marker='', color='#d7191c', markeredgecolor='white', markeredgewidth=1, markersize=10, label='Best Perf. M>1'),
    plt.Line2D([], [], marker='', color='blue',markeredgecolor='white', markeredgewidth=1, markersize=10, label='Best Th. Ut.')
]

fig.legend(
            handles=legend_handles,
            loc='upper center', fontsize=8.5,ncols=10, bbox_to_anchor=(0.5, 1.17))

plt.savefig("../plots/iMFAnt_multithread.pdf", bbox_inches='tight')