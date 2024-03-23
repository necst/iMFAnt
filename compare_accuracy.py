import numpy as np

file1 = open('output_matches/protomata__multi_1m_1t.output.txt')

file2 = open('output_matches/protomata__multi_10m_1t.output.txt')

dir = "output_matches/"


datasets = [
    "poweren", "protomata", "bro", "dotstar09", "ranges1", "tcp"
]

merging_factors = [10, 20, 50]

for d in datasets: 

    print("analysing dataset "+d)
    file1 = open(dir + d + "__multi_1m_1t.output.txt")
    l1 = file1.readlines()

    for m in merging_factors:

        print("with merging factor "+str(m))

        file2 = open(dir + d + "__multi_"+str(m)+"m_1t.output.txt")

        l2 = file2.readlines()

        num_1 = 0
        id = 0

        for l in range(len(l1)):
            if(l1[l][0]=="R"):
                if l%m == 0 and l>0:

                    if (num_1 == int(l2[id].split(" ")[2])):
                        ("ok")
                    else:
                        print("ERROR "+str(id))
                        print("current matches at line "+str(l)+": "+str(num_1))
                        print("compared with "+str(int(l2[id].split(" ")[2])))
                    id += 1
                    num_1 = 0
                num_1 = num_1 + int(l1[l].split(" ")[2])

