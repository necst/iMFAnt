#!/usr/bin/python3

import sys
import codecs
import subprocess
import threading
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import shutil
import math

#plt.rcParams.update({'font.sans-serif':'Helvetica','font.size':16})

inputRE = [
    "../mfsa/bro_", 
    "../mfsa/dotstar09_",
    "../mfsa/poweren_",
    "../mfsa/protomata_",
    "../mfsa/ranges1_",
    "../mfsa/tcp_"
]


palette = ["#f0f9e8",
"#bae4bc",
"#7bccc4",
"#43a2ca",
"#0868ac"]

m = [1, 10, 50,100,0]

names = []
plot = []
fend=[]
cv = []
m1=[]
m2=[]
bend=[]
m_plot = []

for f in inputRE:
    for mm in m: 
        file = open(f+str(mm)+"/compilation_time.txt", "r")
        m_plot.append(mm)
        if(mm<10 and mm!=0):
            names.append(f.split("/")[1].split("_")[0]+",M00"+str(mm))
        elif(mm==0):
            names.append(f.split("/")[1].split("_")[0]+",Mall")
        elif (mm<100):
            names.append(f.split("/")[1].split("_")[0]+",M0"+str(mm))
        else:
            names.append(f.split("/")[1].split("_")[0]+",M"+str(mm))
        l = file.readline()
        fend.append(float(l.split(",")[0])/1000)
        cv.append(float(l.split(",")[1])/1000)
        m1.append(float(l.split(",")[2])/1000)
        m2.append(float(l.split(",")[3])/1000)
        bend.append(float(l.split(",")[4])/1000)

plt = plt.figure()

df = pd.DataFrame({'dataset': names, 'FE':fend,'AST to FSA':cv,'ME-single': m1, 'ME-merging': m2, 'BE': bend})

df.to_csv("../raw_results/compilation_time.csv", index=False)

print(df)

names = ["BRO,M1","BRO,M10","BRO,M50","BRO,M100","BRO,Mall",
         "DS9,M1","DS9,M10","DS9,M50","DS9,M100","DS9,Mall",
         "PEN,M1","PEN,M10","PEN,M50","PEN,M100","PEN,Mall",
         "PRO,M1","PRO,M10","PRO,M50","PRO,M100","PRO,Mall",
         "RG1,M1","RG1,M10","RG1,M50","RG1,M100","RG1,Mall",
         "TCP,M1","TCP,M10","TCP,M50","TCP,M100","TCP,Mall"
         ]

df['dataset'] = names

fig = df.plot.bar(x='dataset', y=['FE', 'AST to FSA','ME-single','ME-merging','BE'], 
                  stacked=True, color=palette, rot=70,
                   logy=True,
                  edgecolor='black', linewidth=0.4, width=1, figsize=(10,4), ylim=(10**-1,90000),
                  xlabel='Datasets', ylabel='Time [ms] - log. scale')
                  
fig.legend(loc='upper center',framealpha=1, ncol=5, fontsize=13, bbox_to_anchor=(0.5, 1))


fig.figure.savefig("../plots/compilation_time.pdf", bbox_inches = 'tight')
