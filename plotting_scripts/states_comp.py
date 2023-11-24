import os
import pandas as pd
import numpy as np

folder_base = ["../mfsa/bro_", 
               "../mfsa/dotstar09_", 
               "../mfsa/poweren_", 
               "../mfsa/protomata_", 
               "../mfsa/ranges1_", 
               "../mfsa/tcp_"]

chunks = ["1", "2", "5", "10", "20", "50","100","0"]
one=[]
two = []
five = []
ten = []
twenty = []
fifty = []
hundred = []
zero = []

names = []

for f in folder_base:
    names.append(f.split("/")[2].split("_")[0])
    for c in chunks:
        tot = []
        gained = []
        folder = f + c + "/"
        for fl in os.listdir(folder):
            if("txt" in fl and "compilation" not in fl): 
                g = open(folder + fl, "r")
                for l in g.readlines():
                    gained.append(float(l.split(",")[0]))
                    tot.append(float(l.split(",")[1]))
        if(c == "1"):
            one.append(100-(np.sum(gained)/np.sum(tot))*100)
        elif(c == "2"):
            two.append(100-(np.sum(gained)/np.sum(tot))*100)
        elif(c == "5"):
            five.append(100-(np.sum(gained)/np.sum(tot))*100)
        elif(c == "10"):
            ten.append(100-(np.sum(gained)/np.sum(tot))*100)
        elif(c == "20"):
            twenty.append(100-(np.sum(gained)/np.sum(tot))*100)
        elif(c == "50"):
            fifty.append(100-(np.sum(gained)/np.sum(tot))*100)
        elif(c == "100"):
            hundred.append(100-(np.sum(gained)/np.sum(tot))*100)
        elif(c == "0"):
            zero.append(100-(np.sum(gained)/np.sum(tot))*100)

df = pd.DataFrame(
        {
            "dataset": names,
            "M=2": two,
            "M=5": five,
            "M=10": ten,
            "M=20": twenty,
            "M=50": fifty,
            "M=100": hundred,
            "M=all": zero
        }
)

df.to_csv("../raw_results/states_comp.csv")