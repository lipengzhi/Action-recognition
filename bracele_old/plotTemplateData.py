#-*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import csv

def read_from_file(path, s_idx, e_idx):
    all_data = []
    cnt = 0
    with open(path, 'r') as fp:
        for line in fp.readlines():
            if cnt >= s_idx and cnt <= e_idx:
                data = []
                row = line.strip().split(',')
                data.append(float(row[1]))
                data.append(float(row[2]))
                data.append(float(row[3]))
                all_data.append(np.array(data))
            cnt += 1
    return np.array(all_data)


def read_from_preProcessed_csv(path):
    all_data = []
    all_zscaleData = []
    with open(path, 'r') as fp:
        reader = csv.reader(fp)
        for line in reader:
            data = []
            zscaleData = []
            if len(line) == 7:
            # data.append(float(line))
                data.append(float(line[0]))
                data.append(float(line[1]))
                data.append(float(line[2]))
                all_data.append(np.array(data))
                zscaleData.append(float(line[4]))
                zscaleData.append(float(line[5]))
                zscaleData.append(float(line[6]))
                all_zscaleData.append(np.array(zscaleData))
    return np.array(all_data),np.array(all_zscaleData)


def read_from_csv(path):
    # all_data = []
    all_zscaleData = []
    with open(path, 'r') as fp:
        reader = csv.reader(fp)
        for line in reader:
            # data = []
            zscaleData = []
            if len(line) == 1:
            # # data.append(float(line))
            #     data.append(float(line[0]))
            #     data.append(float(line[1]))
            #     data.append(float(line[2]))
            #     all_data.append(np.array(data))
                zscaleData.append(float(line[0]))
                # zscaleData.append(float(line[5]))
                # zscaleData.append(float(line[6]))
                all_zscaleData.append(np.array(zscaleData))
    return np.array(all_zscaleData)




def timeSeq2D(data, filename, start = 0, end =  - 1):
    if end == -1:
        end = len(data)



def plotSeq(data, filename, start = 0, end = - 1):
    if end == -1:
        end = len(data)

    fig = plt.figure(filename)
    title = ['x-t', 'y-t', 'z-t']
    ax = fig.add_subplot(2, 2, 1)
    ax.set_title(title[0])
    ax.set_xlabel('t')
    ax.set_ylabel('x')
    ax.scatter(np.arange(start + 1, end + 1, 1), data[start:end, 0], linewidth=0.1)

    ax = fig.add_subplot(2, 2, 2)
    ax.set_title(title[1])
    ax.set_xlabel('t')
    ax.set_ylabel('y')
    ax.scatter(np.arange(start + 1, end + 1, 1), data[start:end, 1], linewidth=0.1)

    ax = fig.add_subplot(2, 2, 3)
    ax.set_title(title[2])
    ax.set_xlabel('t')
    ax.set_ylabel('z')
    ax.scatter(np.arange(start + 1, end + 1, 1), data[start:end, 2], linewidth=0.1)

    ax = fig.add_subplot(2, 2, 4, projection='3d')
    ax.set_title("3D scatter")
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('z')
    length = end-start
    ax.scatter(data[start:start+length // 3, 0], data[start:start+length // 3, 1], data[start:start+length // 3, 2], c='r', linewidth=0.2)
    ax.scatter(data[start+length//3-1:start+length//3*2, 0], data[start+length//3-1:start+length//3*2, 1],
                       data[start+length//3-1:start+length // 3 * 2, 2], c='g', linewidth=0.2)
    ax.scatter(data[start+length//3*2-1:end, 0], data[start+length//3*2-1:end, 1], data[start+length//3*2-1:end, 2], c='b',
                       linewidth=0.2)
    plt.show(fig)

#left_forward = "/Users/guobo/smart_bracelet/Action_Library/moneyCat/templates/left_forward1.csv"
#left_forward = "/Users/guobo/smart_bracelet/Action_Library/moneyCat/templates/left_forward.csv"
#left_forward = "/Users/guobo/smart_bracelet/Action_Library/moneyCat/templates/left_backward.csv"
left_forward = "/Users/guobo/smart_bracelet/Action_Library/Robotarm1/templates/1.csv"
#left_forward = "/Users/guobo/smart_bracelet/Action_Library/moneyCatLeftHand/Log_2018_07_02_11_27_39_inittemplate.csv"

if __name__ == "__main__":

    #show template data
    (left_forward_data, left_forward_data_zscale) = read_from_preProcessed_csv(left_forward)
    # (left_backward_data, left_backward_data_zscale) = read_from_preProcessed_csv(left_backward)
    # (right_forward_data, right_forward_data_zscale) = read_from_preProcessed_csv(right_forward)
    # (right_backward_data, right_backward_data_zscale) = read_from_preProcessed_csv(right_backward)
    plotSeq(left_forward_data, "left_forward")
    plotSeq(left_forward_data_zscale, "left_forward_zscale")


    #show real action and test action.
    # real_data = read_from_file(testData_path, real_action_s[19], real_action_e[19])
    # test_data = read_from_file(testData_path, test_action_s[0], test_action_e[0])
    # plotSeq(real_data, 'real_data')
    # plotSeq(test_data, 'test_data')








