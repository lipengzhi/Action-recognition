import numpy as np
import h5py
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

#样本数据：AALTD6

def dset2npy(fp, dataset_name):
    read_dataset = fp[dataset_name]
    npy_arr = np.empty(read_dataset.shape, dtype=read_dataset.dtype)
    read_dataset.read_direct(npy_arr)
    return npy_arr


def h5_to_np(dataset_name):
    fp = h5py.File(dataset_name, "r")
    x = dset2npy(fp, "time_series")
    y = dset2npy(fp, "labels")
    fp.close()
    return x, y


#画出一个样本51个时间序列的轨迹
def plot_track_left_hand(track_data, sample_number, label):
    # 4张图
    fig = plt.figure()
    ax = fig.add_subplot(2, 2, 1, projection='3d')
    track_x = track_data[:, 0]
    track_y = track_data[:, 1]
    track_z = track_data[:, 2]
    ax.set_title("left hand 3D")
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('z')
    figrure1 = ax.plot(track_x[:18], track_y[:18], track_z[:18], c='r')
    figrure2 = ax.plot(track_x[17:35], track_y[17:35], track_z[17:35], c='b')
    figrure3 = ax.plot(track_x[34:], track_y[34:], track_z[34:], c='g')

    ax = fig.add_subplot(2, 2, 2)
    ax.set_title("left hand x-y")
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    figrure1 = ax.plot(track_x[:18], track_y[:18], c='r')
    figrure2 = ax.plot(track_x[17:35], track_y[17:35], c='b')
    figrure3 = ax.plot(track_x[34:], track_y[34:], c='g')

    ax = fig.add_subplot(2, 2, 3)
    ax.set_title("left hand x-z")
    ax.set_xlabel('x')
    ax.set_ylabel('z')
    figrure1 = ax.plot(track_x[:18], track_z[:18], c='r')
    figrure2 = ax.plot(track_x[17:35], track_z[17:35], c='b')
    figrure3 = ax.plot(track_x[34:], track_z[34:], c='g')

    ax = fig.add_subplot(2, 2, 4)
    ax.set_title("left hand y-z")
    ax.set_xlabel('y')
    ax.set_ylabel('z')
    figrure1 = ax.plot(track_y[:18], track_z[:18], c='r')
    figrure2 = ax.plot(track_y[17:35], track_z[17:35], c='b')
    figrure3 = ax.plot(track_y[34:], track_z[34:], c='g')

    save_name = "./result_3D_and_2D/sample_" + str(sample_number+1) + "_classification_" + str(label)
    # print(save_name)
    title_name = "sample:" + str(sample_number+1) + "    classification:" + str(label)
    print(title_name)
    fig.suptitle(title_name)
    plt.savefig(save_name)
    # plt.close()
    #  plt.show()


def save_show():
    dataset_name = "./dataset/train.h5"
    data, labels = h5_to_np(dataset_name)
    # print(data.shape)
    # print(labels.shape)
    # print(data[0, 0, :])
    k = 180
    # 表示打印的图像个数
    for i in range(k):
        sample_data = data[i, :, :3]
        print(sample_data.shape)
        plot_track_left_hand(sample_data, i, labels[i])




if __name__ == "__main__":
    save_show()



