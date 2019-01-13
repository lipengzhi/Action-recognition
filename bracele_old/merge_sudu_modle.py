#-*- coding: utf-8 -*-

import numpy as np
import os
import sys
import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D
import datasetDealing
import plotTemplateData
from sklearn.svm import SVR
from sklearn.svm import NuSVR
from sklearn.kernel_ridge import KernelRidge
from sklearn.model_selection import GridSearchCV
from sklearn.gaussian_process.kernels import WhiteKernel, ExpSineSquared
import pandas as pd
import csv

# 读取数据
def read_from_file(path):
    all_data = []
    with open(path) as fp:
        for line in fp.readlines():
            # print(line)
            data = []
            row = line.strip().split(',')
            data.append(float(row[1]))
            data.append(float(row[2]))
            data.append(float(row[3]))
            all_data.append(np.array(data))
    return np.array(all_data)


def move_coordinate(data):
    mean_x = data[:, 0].mean()
    mean_y = data[:, 1].mean()
    mean_z = data[:, 2].mean()
    length = len(data)
    newdata = np.zeros((length, 3))
    newdata[:, 0] = data[:, 0] - mean_x
    newdata[:, 1] = data[:, 1] - mean_y
    newdata[:, 2] = data[:, 2] - mean_z
    return newdata


def read_loc_info(loc_path):
    all_data = []
    with open(loc_path) as fp:
        for line in fp.readlines():
            data = []
            row = line.strip().split(', ')
            if len(row) == 4:
                data.append(int(row[0]))
                data.append(int(row[1]))
                all_data.append(np.array(data))
    return np.array(all_data)


def single_merge(aa):

    start = 0.0
    end = 10.0
    first = aa
    template_name = first + '_template_50' + '.csv'
    template_dir = "/Users/lipengzhi/Desktop/sudu_modle/"
    data_list = os.listdir(template_dir)
    print('121',data_list)
    data_list.sort()
    substr = '.csv'
    all_t = np.array([])
    all_x = np.array([])

    for action_name in data_list:
        print('33',action_name)
        if action_name.find(substr) < 0:
            continue
        path_action = template_dir + action_name
        zscale_data = plotTemplateData.read_from_csv(path_action)
        print('444',zscale_data)

        t = np.linspace(start, end, len(zscale_data))
        all_t = np.append(all_t, t)
        print('all_t',all_t)

        x = zscale_data
        print(zscale_data)
        all_x = np.append(all_x, x)
        print('all_x',all_x)


    # fig = plt.figure(1)
    ax1 = plt.subplot(3, 1, 1)
    ax1.scatter(all_t, all_x, 2)


    svrX_rbf = SVR(kernel='rbf', C=1, gamma='auto', epsilon=0.01)  # 径向基X

    all_tt = all_t.reshape(len(all_t), 1)
    print('1211',all_tt)
    svrX_rbf.fit(all_tt, all_x)

    test_t = np.linspace(start, end)
    test_tt = test_t.reshape(len(test_t), 1)
    x_rbf = svrX_rbf.predict(test_tt)
    print(len(x_rbf))


    lw = 2
    ax1.plot(test_t, x_rbf, color='navy', lw=lw)

    print("end")

    speed =['speed']
    for i in range(len(x_rbf)):
        speed.append(x_rbf[i])
    print(speed)

    with open('/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/109_template_50.csv') as csvfile:
        rows = csv.reader(csvfile)
        with open('/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/109_template_501.csv', 'w', newline='') as f:
            writer = csv.writer(f)
            i = 0
            for row in rows:
                if i <=len(x_rbf):
                   row.append(speed[i])
                   i = i + 1
                else:
                    row.append('')
                writer.writerow(row)


    plt.show()

if __name__ == "__main__":
    select_num = 2
    if(select_num == 2):
        first_num = '109'
        single_merge(first_num)

