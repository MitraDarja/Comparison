import sys

import numpy as np
import matplotlib.pyplot as plt
import numpy as np

#k_size = [16,20,24,28,32]
#pos = [x+0.25 for x in range(len(k_size))]
#strobe_range = [int(k/2) for k in k_size]
k_size = [16,20,24,28,32]
pos = [x+0.25 for x in range(len(k_size))]
pos_order3 = [1.25,4.25,7.25]
k_order3 = [6,8,10]
k_size_order3 = [i*3 for i in k_order3]
strobe_range = [k for k in range(8,17,2)]

def read_file(results, files):
    cov = 0.0
    for file in files:
        with open(file, 'r') as f:
            for line in f:
                if (line[:7]=="Match C"):
                    cov = round(float(line.split()[2]),2)
                if (line[:7]=="Islands"):
                    mean = round(float(line.split('\t')[1]),2)
                    stdev = round(float(line.split('\t')[2]),2)
                    results.append((mean,stdev,cov))
    return results

# Read all files for an error
for error in [1,2,5,10]:
    kmers = read_file([], ["0_minimiser_hash_"+str(k)+"_"+str(k)+"_match_"+str(error)+".out" for k in range(16,36,4)])
    shapes4=['777695', '16252901', '251620351', '4286578095', '66035113981']
    gapped4_kmers = read_file([], [shapes4[i] + "_minimiser_hash_"+str(k_size[i]+4)+"_"+str(k_size[i]+4)+"_match_"+str(error)+".out" for i in range(len(k_size))])
    shapes8 = ['14021527', '180082591', '3522001919', '64423783901', '1094946651927']
    gapped8_kmers = read_file([], [shapes8[i] + "_minimiser_hash_"+str(k_size[i]+8)+"_"+str(k_size[i]+8)+"_match_"+str(error)+".out" for i in range(len(k_size))])

    minstrobemers2 = read_file([], ["minstrobemers_"+str(k)+"_2_"+str(1)+"_"+str(4+k)+"_match_"+str(error)+".out" for k in strobe_range])
    hybridstrobemers2 = read_file([],["hybridstrobemers_"+str(k)+"_2_"+str(1)+"_"+str(4+k)+"_match_"+str(error)+".out" for k in strobe_range])
    randstrobemers2 = read_file([], ["randstrobemers_"+str(k)+"_2_"+str(1)+"_"+str(4+k)+"_match_"+str(error)+".out" for k in strobe_range])
    minstrobemers28 = read_file([], ["minstrobemers_"+str(k)+"_2_"+str(1)+"_"+str(8+k)+"_match_"+str(error)+".out" for k in strobe_range])
    hybridstrobemers28 = read_file([],["hybridstrobemers_"+str(k)+"_2_"+str(1)+"_"+str(8+k)+"_match_"+str(error)+".out" for k in strobe_range])
    randstrobemers28 = read_file([], ["randstrobemers_"+str(k)+"_2_"+str(1)+"_"+str(8+k)+"_match_"+str(error)+".out" for k in strobe_range])


    # Plot comparison between all Island size
    fig = plt.figure()
    X = np.arange(len(k_size))
    colors = ["#004c6d","#009dbe","#00f6ff","#fdcc8a","#fc8d59","#d7301f"]
    plt.xlabel("k")
    plt.xticks(pos, k_size)
    plt.ylabel("Average island size")
    plt.plot(pos, [x[0] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos, [x[0] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos, [x[0] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos, [x[0] for x in minstrobemers2], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos, [x[0] for x in hybridstrobemers2], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos, [x[0] for x in randstrobemers2], color = colors[5], label='randstrobemers',linewidth=3.0)
    plt.legend(loc = "upper left", title="Methods")
    plt.savefig("../results/Match_island_"+str(error)+".png",bbox_inches='tight')

    # Plot comparison between all Island size 8
    fig = plt.figure()
    X = np.arange(len(k_size))
    plt.xlabel("k")
    plt.xticks(pos, k_size)
    plt.ylabel("Average island size")
    plt.plot(pos, [x[0] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos, [x[0] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos, [x[0] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos, [x[0] for x in minstrobemers28], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos, [x[0] for x in hybridstrobemers28], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos, [x[0] for x in randstrobemers28], color = colors[5], label='randstrobemers',linewidth=3.0)
    plt.legend(loc = "upper left", title="Methods")
    plt.savefig("../results/Match_island_8_"+str(error)+".png",bbox_inches='tight')

    # Plot comparison between all match coverage
    fig = plt.figure()
    X = np.arange(len(k_size))

    colors = ["#004c6d","#009dbe","#00f6ff","#fdcc8a","#fc8d59","#d7301f"]
    plt.xlabel("k")
    plt.xticks(pos, k_size)
    plt.ylabel("Match coverage")

    plt.plot(pos, [x[2] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos, [x[2] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos, [x[2] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos, [x[2] for x in minstrobemers2], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos, [x[2] for x in hybridstrobemers2], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos, [x[2] for x in randstrobemers2], color = colors[5], label='randstrobemers',linewidth=3.0)
    plt.legend(loc = "upper right", title="Methods")
    plt.savefig("../results/Match_cov_"+str(error)+".png",bbox_inches='tight')

    fig = plt.figure()
    X = np.arange(len(k_size))
    plt.xlabel("k")
    plt.xticks(pos, k_size)
    plt.ylabel("Match coverage")

    plt.plot(pos, [x[2] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos, [x[2] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos, [x[2] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos, [x[2] for x in minstrobemers28], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos, [x[2] for x in hybridstrobemers28], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos, [x[2] for x in randstrobemers28], color = colors[5], label='randstrobemers',linewidth=3.0)
    plt.legend(loc = "upper right", title="Methods")
    plt.savefig("../results/Match_cov_8_"+str(error)+".png",bbox_inches='tight')

    # Plot comparison between all match coverage order 3
    kmers = read_file([], ["0_minimiser_hash_"+str(k)+"_"+str(k)+"_match_"+str(error)+".out" for k in [18,24,30]])
    shapes4=["2621175", "251620351", "13958643693"]
    gapped4_kmers = read_file([], [shapes4[i] + "_minimiser_hash_"+str(k_size_order3[i]+4)+"_"+str(k_size_order3[i]+4)+"_match_"+str(error)+".out" for i in range(len(k_size_order3))])
    shapes8 = ["45607667", "3522001919", "205814423455"]
    gapped8_kmers = read_file([], [shapes8[i] + "_minimiser_hash_"+str(k_size_order3[i]+8)+"_"+str(k_size_order3[i]+8)+"_match_"+str(error)+".out" for i in range(len(k_size_order3))])

    minstrobemers3 = read_file([], ["minstrobemers_"+str(k)+"_3_"+str(1)+"_"+str(4+k)+"_match_"+str(error)+".out" for k in k_order3])
    hybridstrobemers3 = read_file([],["hybridstrobemers_"+str(k)+"_3_"+str(1)+"_"+str(4+k)+"_match_"+str(error)+".out" for k in k_order3])
    randstrobemers3 = read_file([], ["randstrobemers_"+str(k)+"_3_"+str(1)+"_"+str(4+k)+"_match_"+str(error)+".out" for k in k_order3])
    minstrobemers38 = read_file([], ["minstrobemers_"+str(k)+"_3_"+str(1)+"_"+str(8+k)+"_match_"+str(error)+".out" for k in k_order3])
    hybridstrobemers38 = read_file([],["hybridstrobemers_"+str(k)+"_3_"+str(1)+"_"+str(8+k)+"_match_"+str(error)+".out" for k in k_order3])
    randstrobemers38 = read_file([], ["randstrobemers_"+str(k)+"_3_"+str(1)+"_"+str(8+k)+"_match_"+str(error)+".out" for k in k_order3])

    fig = plt.figure()
    X = np.arange(len(k_size_order3))
    plt.xlabel("k")
    plt.xticks(pos_order3, k_order3)
    plt.ylabel("Match coverage")

    plt.plot(pos_order3, [x[2] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in minstrobemers3], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in hybridstrobemers3], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in randstrobemers3], color = colors[5], label='randstrobemers',linewidth=3.0)

    plt.legend(loc = "upper right", title="Methods")
    plt.savefig("../results/Match_cov_3_"+str(error)+".png",bbox_inches='tight')

    fig = plt.figure()
    X = np.arange(len(k_size_order3))
    plt.xlabel("k")
    plt.xticks(pos_order3, k_order3)
    plt.ylabel("Match coverage")

    plt.plot(pos_order3, [x[2] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in minstrobemers38], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in hybridstrobemers38], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[2] for x in randstrobemers38], color = colors[5], label='randstrobemers',linewidth=3.0)

    plt.legend(loc = "upper right", title="Methods")
    plt.savefig("../results/Match_cov_38_"+str(error)+".png",bbox_inches='tight')

    # Plot comparison between all Island size order 3
    fig = plt.figure()
    X = np.arange(len(k_size_order3))
    colors = ["#004c6d","#009dbe","#00f6ff","#fdcc8a","#fc8d59","#d7301f"]
    plt.xlabel("k")
    plt.xticks(pos_order3, k_size_order3)
    plt.ylabel("Average island size")
    plt.plot(pos_order3, [x[0] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in minstrobemers3], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in hybridstrobemers3], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in randstrobemers3], color = colors[5], label='randstrobemers',linewidth=3.0)
    plt.legend(loc = "upper left", title="Methods")
    plt.savefig("../results/Match_island_3_"+str(error)+".png",bbox_inches='tight')

    fig = plt.figure()
    X = np.arange(len(k_size_order3))
    colors = ["#004c6d","#009dbe","#00f6ff","#fdcc8a","#fc8d59","#d7301f"]
    plt.xlabel("k")
    plt.xticks(pos_order3, k_size_order3)
    plt.ylabel("Average island size")
    plt.plot(pos_order3, [x[0] for x in kmers], color = colors[0], label='k-mer', linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in gapped4_kmers], color = colors[1], label='4 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in gapped8_kmers], color = colors[2], label='8 k-mer',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in minstrobemers38], color = colors[3], label='minstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in hybridstrobemers38], color = colors[4], label='hybridstrobemers',linewidth=3.0)
    plt.plot(pos_order3, [x[0] for x in randstrobemers38], color = colors[5], label='randstrobemers',linewidth=3.0)
    plt.legend(title="Methods", loc = "upper left")
    plt.savefig("../results/Match_island_38_"+str(error)+".png",bbox_inches='tight')
