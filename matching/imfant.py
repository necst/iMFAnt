import numpy as np

import subprocess
import os

num_threads = [1, 2, 4, 8, 16, 32, 64, 128]

merging_factors = [0, 1, 2, 5, 10, 20, 50, 100]

output_folder = "../output_matches/"

input = [
    "../mfsa/poweren_", 
    "../mfsa/protomata_",
    "../mfsa/dotstar09_",
    "../mfsa/tcp_",
    "../mfsa/bro_",
    "../mfsa/ranges1_",
]

streams = [
    "../input_streams/input_poweren_1Mb.input",
    "../input_streams/input_proto_1Mb.input",
    "../input_streams/input_dotstar_1Mb.input",
    "../input_streams/input_tcp_1Mb.input",
    "../input_streams/input_bro_1Mb.input",
    "../input_streams/input_ranges_1Mb.input",
]

for i in input: 
    for m in merging_factors:
        for n in num_threads:
            mfsa_dir = i + str(m) + "/"
            output_file = output_folder + i.split("/")[2] + "_multi_"+str(m)+"m_"+str(n)+"t.output.txt"
            num = len(os.listdir(mfsa_dir)) - 1
            p = subprocess.Popen("export OMP_NUM_THREADS="+str(n)+"; make && ./multithreaded_imfant " + streams[input.index(i)] + " " + mfsa_dir + " " + str(num) + " " + output_file, shell=True)
            output, error = p.communicate()

