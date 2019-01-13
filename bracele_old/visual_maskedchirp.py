import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import axes3d
from decimal import *

query_1d = np.loadtxt("/Users/guobo/smart_bracelet/spring/query.dat")
maskedchirp_1d = np.loadtxt("/Users/guobo/smart_bracelet/spring/maskedchirp.dat")
#query_3d = np.load()
#maskedchirp_3d = np.load()
np.set_printoptions(suppress=True)

if __name__ == "__main__":
    query_1d_idx = [n for n in range(len(query_1d))]
    maskedchirp_1d_idx = [n for n in range(len(maskedchirp_1d))]

    x_q = query_1d_idx
    y_q = x_q
    z_q = query_1d
    x_m = maskedchirp_1d_idx
    y_m = x_m
    z_m = maskedchirp_1d

    query_3d_dat = np.vstack((x_q, y_q, z_q))
    query_3d_dat = np.array(query_3d_dat).reshape(3, len(x_q))
    query_3d_dat = np.transpose(query_3d_dat)
    np.savetxt("/Users/guobo/smart_bracelet/spring/query_3d.dat", query_3d_dat, fmt="%.5f")

    masked_3d_dat = np.vstack((x_m, y_m, z_m))
    masked_3d_dat = np.array(masked_3d_dat).reshape(3, len(x_m))
    masked_3d_dat = np.transpose(masked_3d_dat)
    np.savetxt("/Users/guobo/smart_bracelet/spring/masked_3d.dat", masked_3d_dat, fmt="%.5f")

    fig = plt.figure()
    ax = fig.add_subplot(2, 2, 1)
    ax.set_title("query")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.plot(query_1d_idx, query_1d, c='r')

    ax = fig.add_subplot(2, 2, 2)
    ax.set_title("maskedchirp")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.plot(maskedchirp_1d_idx, maskedchirp_1d, c='g')

    ax = fig.add_subplot(2, 2, 3, projection='3d')
    ax.set_title("query_3d")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    ax.plot(x_q, y_q, z_q, c='r')

    ax = fig.add_subplot(2, 2, 4, projection='3d')
    ax.set_title("maskedchirp_3d")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    ax.plot(x_m, y_m, z_m, c='g')

    plt.show()
    plt.clf()
    plt.close()

    print("Done.")