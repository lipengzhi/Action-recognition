# !/usr/bin/python
# -*- coding: utf-8 -*-

"""Track data copy from mobile to computer's action folder
Version 0.2
"""

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


# dataset copy to folder named "action"
def adb_pull(dataDate):
    dataset_dir = os.path.abspath('.') + os.sep + "action/"  # 数据存放文件夹，默认为程序所在目录的action下

    if not os.path.exists(dataset_dir):  # 不存在该文件夹，新建
        os.mkdir(dataset_dir)

    dataset_dir += "2018_" + dataDate + "/"

    print(dataset_dir)

    if not os.path.exists(dataset_dir):  # 不存在该文件夹，新建
        os.mkdir(dataset_dir)

    result = os.popen('adb devices').read().strip().split('\n')  # 检测连接情况

    if not(len(result) == 2 and result[1].split('\t')[1] == 'device'):  # 连接不成功
        print("设备连接有误，请在终端下使用adb device命令查看设备连接情况\n")
    else:
        fileName = "/sdcard/cheng/Log_2018_" + dataDate + "**.txt"        # 选取某一天的所有数据，例如：2018_06_14
        adb_command = r"adb shell ls %s | tr '\r' ' ' | xargs -n1 -I {} adb pull {} %s" % (fileName, dataset_dir)
        if os.system(adb_command) != 0:
            print("\ncopy failed")
            sys.exit("sorry, goodbye!\n")
        else:
            if len(os.listdir(dataset_dir)) == 0:
                print("\n\nnot exist this day's data!\n\n")
                os.rmdir(dataset_dir)
            else:
                print("\n\nSuccessful copy to", dataset_dir, "\n\n")


def read_from_file(path):
    all_data = []
    with open(path) as fp:
        for line in fp.readlines():
            data = []
            row = line.strip().split(',')
            # data.append(float(line))
            data.append(float(row[1]))
            data.append(float(row[2]))
            data.append(float(row[3]))
            all_data.append(np.array(data))
    return np.array(all_data)


def plot_time_seq(data, start, end, fig_num, fig_title):
    fig = plt.figure(num=fig_num, figsize=(9, 5))
    fig.suptitle(fig_title)
    title = ['x-t', 'y-t', 'z-t']
    # ax = fig1.add_subplot(3, 1, 1)
    ax1 = plt.subplot2grid((3, 4), (0, 0), colspan=2)
    ax1.set_title(title[0])
    ax1.set_xlabel('t')
    ax1.set_ylabel('x')
    ax1.plot(np.arange(start+1, end+1, 1), data[start:end, 0], linewidth=0.2)

    # ax = fig1.add_subplot(3, 1, 2)
    ax2 = plt.subplot2grid((3, 4), (1, 0), colspan=2)

    ax2.set_title(title[1])
    ax2.set_xlabel('t')
    ax2.set_ylabel('y')
    ax2.plot(np.arange(start+1, end+1, 1), data[start:end, 1], linewidth=0.2)

    # ax = fig1.add_subplot(3, 1, 3)
    ax3 = plt.subplot2grid((3, 4), (2, 0), colspan=2)

    ax3.set_title(title[2])
    ax3.set_xlabel('t')
    ax3.set_ylabel('z')
    ax3.plot(np.arange(start+1, end+1, 1), data[start:end, 2], linewidth=0.2)

    ax = plt.subplot2grid((3, 4), (0, 2), rowspan=3, colspan=2, projection='3d')
    ax.set_title("3D scatter")
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('z')
    length = end - start
    ax.scatter(data[start:start + length // 3, 0], data[start:start + length // 3, 1],
               data[start:start + length // 3, 2], c='r', linewidth=0.2)
    ax.scatter(data[start + length // 3 - 1:start + length // 3 * 2, 0],
               data[start + length // 3 - 1:start + length // 3 * 2, 1],
               data[start + length // 3 - 1:start + length // 3 * 2, 2], c='g', linewidth=0.2)
    ax.scatter(data[start + length // 3 * 2 - 1:end, 0], data[start + length // 3 * 2 - 1:end, 1],
               data[start + length // 3 * 2 - 1:end, 2], c='b', linewidth=0.2)


def plot_label_all(actions_dir):
    data_list = os.listdir(actions_dir)
    data_list.sort()
    for action_name in data_list:
        path_action = actions_dir + action_name
        fig_title = action_name.split('.')[0]
        per_data = read_from_file(path_action)
        flag = True
        while flag:
            num = 1
            plot_time_seq(per_data, 0, len(per_data), num, fig_title + "_All")  # 整体时间序列图
            num = 2
            print("\ninput start point and end point in (0, %d), if start = end, Exit the loop\n" % len(per_data))
            start = int(input("input start point:"))
            end = int(input("input end point:"))
            if start == end:
                break
            plot_time_seq(per_data, start, end, num, fig_title + "_part")  # 部分时间序列图
            plt.show()


if __name__ == "__main__":
    print("\n1. Pull data from mobile.\n2. Plot picture and Label data.\n0: Exit.\n")
    number = 0
    while True:
        number = int(input("input a number 1 or 2 or 0:"))
        if number == 1:
            actionDate = input('Enter month and day to pull data (eg: 06_14): ')
            adb_pull(actionDate)
        elif number == 2:
            print("\nploting picture and labeling data...\n")
            actionDate = input('Enter month and day to pull data (eg: 06_14): ')
            action_dir = os.path.abspath('.') + os.sep + "action/2018_" + actionDate + "/"
            plot_label_all(action_dir)
            break
        else:
            print("input error")
            break





