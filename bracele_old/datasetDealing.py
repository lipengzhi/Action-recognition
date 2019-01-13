# !/usr/bin/python
# -*- coding: utf-8 -*-

"""Track data copy from mobile to computer's action folder
Version 0.1
"""

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import math
import scipy.signal as signal
from mpl_toolkits.mplot3d import Axes3D
import string
from decimal import *


# g_separator = '\t'
# g_index = 0
g_separator = ','
g_index = 1
# dataset copy to folder.
def adb_pull(extract_path, dataDate):
    dataset_dir = extract_path  # 数据存放文件夹

    if not os.path.exists(dataset_dir):  # 不存在该文件夹，新建
        os.mkdir(dataset_dir)
    print(dataset_dir)

    result = os.popen('adb devices').read().strip().split('\n')  # 检测连接情况

    if not(len(result) == 2 and result[1].split('\t')[1] == 'device'):  # 连接不成功
        print("设备连接有误，请在终端下使用adb device命令查看设备连接情况")
    else:
        fileName = "/sdcard/cheng/Log_" + dataDate + "**.txt"        # 选取某一天的所有数据，例如：2018_06_14
        adb_command = r"adb shell ls %s | tr '\r' ' ' | xargs -n1 -I {} adb pull {} %s" % (fileName, dataset_dir)
        if os.system(adb_command) != 0:
            print("copy failed")
            sys.exit("sorry, goodbye!")
        else:
            if len(os.listdir(dataset_dir)) == 0:
                print("not exist this day's data!")
                os.rmdir(dataset_dir)
                sys.exit("sorry, goodbye!")
            else:
                print("Successful copy to", dataset_dir)


def read_from_file(path, index):
    all_data = []
    with open(path) as fp:
        for line in fp.readlines():
            data = []
            row = line.strip().split(g_separator)
            x = "%.3f" % float(row[index])
            y = "%.3f" % float(row[index+1])
            z = "%.3f" % float(row[index+2])
            print('x',x)
            # data.append(float(x))
            # data.append(float(y))
            # data.append(float(z))
            data.append(float(row[index]))
            data.append(float(row[index+1]))
            data.append(float(row[index+2]))
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

    ax2 = plt.subplot2grid((3, 4), (1, 0), colspan=2)

    ax2.set_title(title[1])
    ax2.set_xlabel('t')
    ax2.set_ylabel('y')
    ax2.plot(np.arange(start+1, end+1, 1), data[start:end, 1], linewidth=0.2)

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

    ax.grid(True)
    x = [data[:, 0].min(), data[:, 0].max()]
    y = [data[:, 1].min(), data[:, 1].max()]
    z = [data[:, 2].min(), data[:, 2].max()]
    t = [0, 0]
    ax.plot(t, t, z, c='k')
    ax.plot(x, t, t, c='k')
    ax.plot(t, y, t, c='k')


def plot_all_data(actions_dir, start, end):
    data_list = os.listdir(actions_dir)
    data_list.sort()
    substr = '.txt'
    for action_name in data_list:
        if action_name.find(substr) < 0:
            continue
        path_action = actions_dir + action_name
        fig_title = action_name.split('.')[0]
        if fig_title == '':
            continue
        per_data = read_from_file(path_action, g_index)
        num = 1
        plot_time_seq(per_data, 0, len(per_data), num, fig_title + "_All")  # 整体时间序列图
        num += 1
        plot_time_seq(per_data, start, end, num, fig_title + "_part")  # 部分时间序列图
        plt.show()

def plot_label_data(label_file, start_idxs, end_idxs):
    data = read_from_file(label_file, g_index)
    fig = plt.figure(num=1, figsize=(9, 5))
    fig.suptitle('label-data')
    title = ['x-t', 'y-t', 'z-t']
    ax1 = plt.subplot2grid((3, 4), (0, 0), colspan=2)
    ax1.set_title(title[0])
    ax1.set_xlabel('t')
    ax1.set_ylabel('x')
    for i in range(len(start_idxs)):
        if i%2 == 0:
            color_ = 'red'
        else:
            color_ = 'blue'
        ax1.plot(np.arange(start_idxs[i], end_idxs[i], 1), data[start_idxs[i]:end_idxs[i], 0], color=color_, linewidth=0.2)

    ax2 = plt.subplot2grid((3, 4), (1, 0), colspan=2)
    ax2.set_title(title[1])
    ax2.set_xlabel('t')
    ax2.set_ylabel('y')
    for i in range(len(start_idxs)):
        if i%2 == 0:
            color_ = 'red'
        else:
            color_ = 'blue'
        ax2.plot(np.arange(start_idxs[i], end_idxs[i], 1), data[start_idxs[i]:end_idxs[i], 1], color=color_, linewidth=0.2)

    ax3 = plt.subplot2grid((3, 4), (2, 0), colspan=2)
    ax3.set_title(title[2])
    ax3.set_xlabel('t')
    ax3.set_ylabel('z')
    for i in range(len(start_idxs)):
        if i%2 == 0:
            color_ = 'red'
        else:
            color_ = 'blue'
        ax3.plot(np.arange(start_idxs[i], end_idxs[i], 1), data[start_idxs[i]:end_idxs[i], 2], color=color_, linewidth=0.2)

    ax = plt.subplot2grid((3, 4), (0, 2), rowspan=3, colspan=2, projection='3d')
    ax.set_title("3D scatter")
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('z')
    length = end_idxs[0] - start_idxs[0] + 1
    start = start_idxs[0]
    ax.scatter(data[start:start + length // 3, 0], data[start:start + length // 3, 1],
               data[start:start + length // 3, 2], c='r', linewidth=0.2)
    ax.scatter(data[start + length // 3 - 1:start + length // 3 * 2, 0],
               data[start + length // 3 - 1:start + length // 3 * 2, 1],
               data[start + length // 3 - 1:start + length // 3 * 2, 2], c='g', linewidth=0.2)
    ax.scatter(data[start + length // 3 * 2 - 1:end, 0], data[start + length // 3 * 2 - 1:end, 1],
               data[start + length // 3 * 2 - 1:end, 2], c='b', linewidth=0.2)

    ax.grid(True)
    x = [data[:, 0].min(), data[:, 0].max()]
    y = [data[:, 1].min(), data[:, 1].max()]
    z = [data[:, 2].min(), data[:, 2].max()]
    t = [0, 0]
    ax.plot(t, t, z, c='k')
    ax.plot(x, t, t, c='k')
    ax.plot(t, y, t, c='k')
    plt.show()


def read_file(path, index):
    # all_data = []
    data_x = []
    data_y = []
    data_z = []
    with open(path) as fp:
        for line in fp.readlines():
            row = line.strip().split(g_separator)
            x = "%.3f" % float(row[index])
            y = "%.3f" % float(row[index+1])
            z = "%.3f" % float(row[index+2])
            data_x.append(float(row[index]))
            data_y.append(float(row[index+1]))
            data_z.append(float(row[index+2]))
    # print('data_x',data_x)
    # print('data_y',data_y)
    # print('data_z',data_z)
    return data_x,data_y,data_z

def read_lable ():
    start_la = np.loadtxt('/Users/lipengzhi/Desktop/234/109/nan_lable.txt',usecols= 0)
    end_la = np.loadtxt('/Users/lipengzhi/Desktop/234/109/nan_lable.txt', usecols=1)

    # start_la = np.loadtxt('/Users/lipengzhi/Desktop/sudu_duibi_la/109/lable.txt', usecols=0)
    # end_la = np.loadtxt('/Users/lipengzhi/Desktop/sudu_duibi_la/109/lable.txt', usecols=1)


    return start_la,end_la



if __name__ == "__main__":

    #number == 1 : extract data from phone;
    #number == 2 : show all data;
    #number == 3 : show label data;
    #number == 4：：计算距离值
    number = 5

    actionDate = "2018_07_02"
    # extract_path = "/Users/guobo/smart_bracelet/Action_Library/15action_xiaoyu_jingyan/tmp/"
    # extract_path = "/Users/lipengzhi/Desktop/brace_letgesturerecognition/tracking/"


    extract_path = "/Users/lipengzhi/Desktop/sudu_duibi/"
    # extract_path = "/Users/lipengzhi/Desktop/caiyang_nu/"

    # extract_path = "/Users/lipengzhi/Desktop/hudu/"
    # extract_path = "/Users/guobo/smart_bracelet/Action_Library/moneyCatLeftHand/"
    start, end = 155,291

    label_file = extract_path + "123coord.txt"
    start_idxs = [116, 1244, 2212, 3236, 4172, 5164, 5997, 6949, 7862, 8790, 9647, 10629, 11490, 12396, 13246]
    end_idxs = [1243, 2211, 3235, 4171, 5163, 5996, 6948, 7861, 8789, 9646, 10628, 11489, 12395, 13245, 14209]
    if number == 1:
        adb_pull(extract_path, actionDate)
    elif number == 2:
        plot_all_data(extract_path, start, end)
    # elif number == 3:
    #     plot_label_data(label_file, start_idxs, end_idxs)
    # elif number == 4:
    #    # data_x,data_y,data_z = read_file('/Users/lipengzhi/Desktop/hudu/1_180.txt',g_index)
    #
    #    data_x, data_y, data_z = read_file('/Users/lipengzhi/Desktop/caiyang_nan2/114.txt', g_index)
    #
    #    # data_x, data_y, data_z = read_file('/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/124coord.txt', g_index)
    #    # print(data_y)
    #    # print(len(data_x))
    #    lens = 0.0
    #    for i in range (1445,1497):
    #
    #        x = data_x[i+1] - data_x[i]
    #        y = data_y[i+1] - data_y[i]
    #        z = data_z[i+1] - data_z[i]
    #        # 用math.sqrt（）求平方根
    #        lens += math.sqrt((x ** 2) + (y ** 2) + (z ** 2))
    #    print (lens)
    # 画距离图像
    elif number == 5:
        start, end = read_lable()
        start_lable = start.astype(int)
        print('www',start_lable)
        end_lable = end.astype(int)
        data_x, data_y, data_z = read_file('/Users/lipengzhi/Desktop/caiyang_nan/109.txt', g_index)

        # data_x, data_y, data_z = read_file('/Users/lipengzhi/Desktop/sudu_duibi/109.txt', g_index)
        # lens = 0.0

        for j in range (0,len(start_lable)):
            lens = 0.0
            distance = []
            print(start_lable[j])
            print(end_lable[j])
            print('s',end_lable[j] - start_lable[j])
            for i in range (start_lable[j],end_lable[j]):
                x = data_x[i+1] - data_x[i]
                y = data_y[i+1] - data_y[i]
                z = data_z[i+1] - data_z[i]
           # 用math.sqrt（）求平方根
                lens = math.sqrt((x ** 2) + (y ** 2) + (z ** 2))
            # print('累加距离为',lens)1
                print(lens)
                distance.append(lens)
            distance_1 = signal.medfilt(distance,3)

            np.savetxt('/Users/lipengzhi/Desktop/duibi_modle/'+ str(j)+'.csv', distance_1, delimiter=',')
            middle = int(len(distance_1)/2)
            distance_fir = distance_1[:middle]
            distance_las = distance_1[middle:]
            print(distance_fir,distance_las)
            max_distance_fir = np.max(distance_fir)

            max_distance_las = np.max(distance_las)
            print('前一个波峰最大值',max_distance_fir)
            print('后一个波峰最大值', max_distance_las)
            print('两个峰值的差距', max_distance_fir - max_distance_las)

            plt.plot(distance_1)  # plot绘制折线图

        plt.show()
#求均值画0-1图
    elif number == 6:
        start, end = read_lable()
        start_lable = start.astype(int)
        end_lable = end.astype(int)
        data_x, data_y, data_z = read_file('/Users/lipengzhi/Desktop/caiyang_nu2/122.txt', g_index)
        # lens = 0.0
        # distance =[]
        for j in range (len(start_lable)):
            lens = 0.0
            distance = []
            print(start_lable[j])
            print(end_lable[j])
            for i in range (start_lable[j],end_lable[j]):
                x = data_x[i+1] - data_x[i]
                y = data_y[i+1] - data_y[i]
                z = data_z[i+1] - data_z[i]
           # 用math.sqrt（）求平方根
                lens = math.sqrt((x ** 2) + (y ** 2) + (z ** 2))
                distance.append(lens)
            distance_1 = signal.medfilt(distance, 3)
            sum_dis = sum(distance_1)
            avge = sum_dis/len(distance_1)
            min_distance_1 = np.min(distance_1)
            min_avge = (min_distance_1 + avge)/2
            for i in range(len(distance_1)):

                if distance_1[i] >= min_avge:
                    distance_1[i] = 1.
                else:
                    distance_1[i] = 0.

            print(distance_1)
            plt.plot(distance_1)  # plot绘制折线图
            # plt.show()

            indexg_s1 =0
            indexg_e1 =0
            for i in range(len(distance_1)):
               if distance_1[i] == 1:
                   indexg_s1 = i
                   break
               if distance_1[i] == 0 and distance_1[i+1] == 1 :
                   indexg_s1 = i+1
                   break
            for i in range(len(distance_1)):
                if distance_1[i] == 1 and distance_1[i+1] == 0 :
                   indexg_e1 = i
                   break
            print('运动时间01',indexg_e1 - indexg_s1)
            indexs_s1 = indexg_e1 +1
            indexs_e1 = 0
            for i in range(len(distance_1)-1):
                if distance_1[i] == 0 and distance_1[i+1] == 1 and i<=105:
                    indexs_e1 = i
            print('静止时间01',indexs_e1 - indexs_s1)
            indexg_s2 = indexs_e1+1
            print(indexg_s2)
            indexg_e2 = 0
            for i in range(len(distance_1)):
                if distance_1[len(distance_1)-1]== 1:
                    indexg_e2 = len(distance_1)
                    break
                if distance_1[i] == 1 and distance_1[i+1] == 0 :
                   indexg_e2 = i
            print('运动时间02',indexg_e2 - indexg_s2)
            plt.show()




    else:
        print("input error, please again!")






