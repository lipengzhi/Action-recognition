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

def for_merge():
    # 遍历标记点csv文件个数
    lable_path = "/Users/lipengzhi/Desktop/yangben/"
    names = os.listdir(lable_path)
    print(names)
    for i in range(len(names)):
        # 文件标号
        first = names[i][0:3]
        template_name = first + '_template_50' + '.csv'
        common_path = "/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/"
        # 拼接源文件名字
        first_name = first + 'coord' + '.txt'
        # first_name加一个空格执行c脚本传参数用
        first_name2 = first + 'coord' + '.txt '
        lable_name = lable_path + first + '.csv'
        # data_path1 = common_path + first_name
        # print("aa",data_path1)
        # 进入目标文件目录执行C++文件
        os.chdir("/Users/lipengzhi/Library/Developer/Xcode/DerivedData/Build/Products/Debug/")
        # 执行c++中main函数case1：  两个参数 1为执行case1；执行的源数据文件地址为data_path1
        os.system('./bracelet_log ' + lable_name + ' 1 ' + first_name2 + template_name)
        # os.popen('./bracelerate_alg ' + '1', 'r').read()
        start = 0.0
        end = 10.0

        # 此处为case1执行完读取sample_count的值
        a = np.loadtxt(common_path + "sample_count.txt")
        nums = int(a)
        print('nums:', nums)
        template_dir = common_path + "templates/"
        data_list = os.listdir(template_dir)
        data_list.sort()
        substr = '.csv'
        all_t = np.array([])
        all_x = np.array([])
        all_y = np.array([])
        all_z = np.array([])
        for action_name in data_list:
            if action_name.find(substr) < 0:
                continue
            path_action = template_dir + action_name
            (org_data, zscale_data) = plotTemplateData.read_from_preProcessed_csv(path_action)

            # zscale_data = zscale_data[0:len(zscale_data)//2]

            t = np.linspace(start, end, len(zscale_data))
            all_t = np.append(all_t, t)

            x = zscale_data[:, 0]
            all_x = np.append(all_x, x)

            y = zscale_data[:, 1]
            all_y = np.append(all_y, y)

            z = zscale_data[:, 2]
            all_z = np.append(all_z, z)

        fig = plt.figure(1)
        ax1 = plt.subplot(3, 1, 1)
        ax1.scatter(all_t, all_x, 2)
        ax2 = plt.subplot(3, 1, 2)
        ax2.scatter(all_t, all_y, 2)
        ax3 = plt.subplot(3, 1, 3)
        ax3.scatter(all_t, all_z, 2)

        svrX_rbf = SVR(kernel='rbf', C=1, gamma='auto', epsilon=0.01)  # 径向基X
        svrY_rbf = SVR(kernel='rbf', C=1, gamma='auto', epsilon=0.01)  # 径向基Y
        svrZ_rbf = SVR(kernel='rbf', C=1e2, gamma=0.1)  # 径向基Z

        all_tt = all_t.reshape(len(all_t), 1)
        svrX_rbf.fit(all_tt, all_x)
        svrY_rbf.fit(all_tt, all_y)
        svrZ_rbf.fit(all_tt, all_z)

        test_t = np.linspace(start, end, nums)
        test_tt = test_t.reshape(len(test_t), 1)
        x_rbf = svrX_rbf.predict(test_tt)
        y_rbf = svrY_rbf.predict(test_tt)
        z_rbf = svrZ_rbf.predict(test_tt)

        # y_rbf = np.zeros(len(y_rbf))
        z_rbf = np.zeros(len(z_rbf))

        # y_poly = svr_poly.fit(all_tt, all_x).predict(test_t.reshape(len(test_t), 1))

        lw = 2
        ax1.plot(test_t, x_rbf, color='navy', lw=lw)
        ax2.plot(test_t, y_rbf, color='navy', lw=lw)
        ax3.plot(test_t, z_rbf, color='navy', lw=lw)

        # ax1.plot(test_t, y_poly, color='cornflowerblue', lw=lw, label='Polynomial model')
        print("end")

        model_x = x_rbf.reshape((len(x_rbf), 1))

        model_y = y_rbf.reshape((len(y_rbf), 1))

        model_z = z_rbf.reshape((len(z_rbf), 1))

        model = np.hstack((model_x, model_y, model_z))

        # save template data
        # template_name = first + '_template_50'+'.csv'
        end_idx = template_dir.rfind('/', 0, len(template_dir) - 1)
        template_path = template_dir[:end_idx + 1] + template_name
        # template_path = template_dir[:end_idx+1] + 'template.csv'
        # dataframe = pd.DataFrame({'zscale-x':x_rbf[::2], 'zscale-y':y_rbf[::2], 'zscale-z':z_rbf[::2]})
        dataframe = pd.DataFrame({'zscale-x': x_rbf, 'zscale-y': y_rbf, 'zscale-z': z_rbf})
        dataframe.to_csv(template_path, index=False, sep=',')
        datasetDealing.plot_time_seq(model, 0, len(model), 2, "model")
        plt.show()
        # 执行c++中main函数case2：
        os.system('./bracelet_log ' + lable_name + ' 2 ' + first_name2 + template_name)


def single_merge(aa):
    # 遍历标记点csv文件个数
    lable_path = "/Users/lipengzhi/Desktop/yangben/"
    # 文件标号
    first = aa
    template_name = first + '_template_50' + '.csv'
    common_path = "/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/"
    first_name = first + 'coord' + '.txt'
    # first_name加一个空格执行c脚本传参数用
    first_name2 = first + 'coord' + '.txt '
    lable_name = lable_path + first + '.csv'
    # data_path1 = common_path + first_name
    # print("aa",data_path1)
    # 进入目标文件目录执行C++文件
    os.chdir("/Users/lipengzhi/Library/Developer/Xcode/DerivedData/Build/Products/Debug/")
    # 执行c++中main函数case1：  两个参数 1为执行case1；执行的源数据文件地址为data_path1
    os.system('./bracelet_log ' + lable_name + ' 1 ' + first_name2 + template_name)
    # os.popen('./bracelerate_alg ' + '1', 'r').read()
    start = 0.0
    end = 10.0
    # 此处为case1执行完读取sample_count的值
    a = np.loadtxt(common_path + "sample_count.txt")
    nums = int(a)
    print('nums:', nums)
    template_dir = common_path + "templates/"
    data_list = os.listdir(template_dir)
    data_list.sort()
    substr = '.csv'
    all_t = np.array([])
    all_x = np.array([])
    all_y = np.array([])
    all_z = np.array([])
    for action_name in data_list:
        if action_name.find(substr) < 0:
            continue
        path_action = template_dir + action_name
        (org_data, zscale_data) = plotTemplateData.read_from_preProcessed_csv(path_action)

        # zscale_data = zscale_data[0:len(zscale_data)//2]

        t = np.linspace(start, end, len(zscale_data))
        all_t = np.append(all_t, t)

        x = zscale_data[:, 0]
        print(zscale_data)
        all_x = np.append(all_x, x)

        y = zscale_data[:, 1]
        all_y = np.append(all_y, y)

        z = zscale_data[:, 2]
        all_z = np.append(all_z, z)

    fig = plt.figure(1)
    ax1 = plt.subplot(3, 1, 1)
    ax1.scatter(all_t, all_x, 2)
    ax2 = plt.subplot(3, 1, 2)
    ax2.scatter(all_t, all_y, 2)
    ax3 = plt.subplot(3, 1, 3)
    ax3.scatter(all_t, all_z, 2)

    svrX_rbf = SVR(kernel='rbf', C=1, gamma='auto', epsilon=0.01)  # 径向基X
    svrY_rbf = SVR(kernel='rbf', C=1, gamma='auto', epsilon=0.01)  # 径向基Y
    svrZ_rbf = SVR(kernel='rbf', C=1e2, gamma=0.1)  # 径向基Z

    all_tt = all_t.reshape(len(all_t), 1)
    svrX_rbf.fit(all_tt, all_x)
    svrY_rbf.fit(all_tt, all_y)
    svrZ_rbf.fit(all_tt, all_z)

    test_t = np.linspace(start, end, nums)
    test_tt = test_t.reshape(len(test_t), 1)
    x_rbf = svrX_rbf.predict(test_tt)
    y_rbf = svrY_rbf.predict(test_tt)
    z_rbf = svrZ_rbf.predict(test_tt)

    # y_rbf = np.zeros(len(y_rbf))
    z_rbf = np.zeros(len(z_rbf))

    # y_poly = svr_poly.fit(all_tt, all_x).predict(test_t.reshape(len(test_t), 1))

    lw = 2
    ax1.plot(test_t, x_rbf, color='navy', lw=lw)
    ax2.plot(test_t, y_rbf, color='navy', lw=lw)
    ax3.plot(test_t, z_rbf, color='navy', lw=lw)

    # ax1.plot(test_t, y_poly, color='cornflowerblue', lw=lw, label='Polynomial model')
    print("end")

    model_x = x_rbf.reshape((len(x_rbf), 1))

    model_y = y_rbf.reshape((len(y_rbf), 1))

    model_z = z_rbf.reshape((len(z_rbf), 1))

    model = np.hstack((model_x, model_y, model_z))

    # save template data
    # template_name = first + '_template_50'+'.csv'
    end_idx = template_dir.rfind('/', 0, len(template_dir) - 1)
    template_path = template_dir[:end_idx + 1] + template_name
    # template_path = template_dir[:end_idx+1] + 'template.csv'
    # dataframe = pd.DataFrame({'zscale-x':x_rbf[::2], 'zscale-y':y_rbf[::2], 'zscale-z':z_rbf[::2]})
    dataframe = pd.DataFrame({'zscale-x': x_rbf, 'zscale-y': y_rbf, 'zscale-z': z_rbf})
    print('32',len(x_rbf))
    dataframe.to_csv(template_path, index=False, sep=',')
    datasetDealing.plot_time_seq(model, 0, len(model), 2, "model")
    plt.show()
    # 执行c++中main函数case2：
    os.system('./bracelet_log ' + lable_name + ' 2 ' + first_name2 + template_name)

if __name__ == "__main__":
    select_num = 2
    print("you select_num ：",select_num)
    if (select_num == 1):
        for_merge()
    if(select_num == 2):
        first_num = '109'
        single_merge(first_num)

