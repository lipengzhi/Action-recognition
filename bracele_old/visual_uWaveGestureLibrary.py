#-*- coding: utf-8 -*
import arff
import os
import shutil
import numpy as np
import string
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt

data_path = '/Users/guobo/smart_bracelet/TSC_motion/UWaveGestureLibraryAll/UWaveGestureLibraryAll_TRAIN.arff'
dir_path = '/Users/guobo/smart_bracelet/uWaveGestureLibrary/U1 (1)'

def extract_data(data_path):
    data = arff.load(open(data_path, 'r'))
    data = data['data']
    print('len(data[0]): %d, data[0][0]: %s, data[0][315]: %s, data[0][-1]: %s.' % (len(data[0]), data[0][0], data[0][315], data[0][-1]))
    data_len = len(data)
    curve_len = int ((len(data[0])-1) / 3)
    x = [[0 for col in range(curve_len)] for row in range(data_len)]
    y = [[0 for col in range(curve_len)] for row in range(data_len)]
    z = [[0 for col in range(curve_len)] for row in range(data_len)]
    class_n = [0]*data_len
    for i in range(data_len):
        if len(data[0]) != len(data[i]):
            print('found %d is error!' % i)
            exit()
        '''
        for j in range(0, data_len, 3):
            idx = int(j/3)
            x[i][idx] = data[i][j]
            y[i][idx] = data[i][j+1]
            z[i][idx] = data[i][j+2]
        '''
        x[i] = data[i][:curve_len]
        y[i] = data[i][curve_len:2*curve_len]
        z[i] = data[i][2*curve_len:3*curve_len]

        class_n[i] = data[i][-1]
    return (x,y,z,class_n)

def get_data_reslut_path(data_path, flag):
    if flag == 0:
        current_path_i = data_path.rfind('/')
        current_path = data_path[:current_path_i]
        dataset_i = data_path.rfind('.')
        dataset_name = data_path[current_path_i + 1:dataset_i]
        result_name = dataset_name + '_result'
        result_path = os.path.join(current_path, result_name)
    elif flag == 1:
        result_path = os.path.join(data_path, 'result_jpg')

    isExits = os.path.exists(result_path)
    if isExits:
        shutil.rmtree(result_path)
    os.makedirs(result_path)
    return result_path

def extract_uwave_org_data(dir_path):
    files = os.listdir(dir_path)
    x = []
    y = []
    z = []
    class_n = []
    for file in files:
        file_path = os.path.join(dir_path, file)
        if not os.path.isdir(file_path) and file.find('.txt') != -1:
            cur_i = file.rfind('-')
            num = int(file[cur_i-1:cur_i])
            f = np.loadtxt(file_path)
            x.append(f[:, 0:1])
            y.append(f[:, 1:2])
            z.append(f[:, 2:3])
            class_n.append(str(num))
    return (x,y,z,class_n)

if __name__ == "__main__":
    result_path = get_data_reslut_path(dir_path, 1)
    print('beginnning to process dataset:')
    #x,y,z,class_n = extract_data(data_path)
    x, y, z, class_n = extract_uwave_org_data(dir_path)
    #t_i = [n for n in range(len(x[0]))]
    margin = int(len(x[0])/3)
    for i in range(len(x)):
        x_i = np.array(x[i])
        x_i = x_i.flatten()
        y_i = np.array(y[i])
        y_i = y_i.flatten()
        z_i = np.array(z[i])
        z_i = z_i.flatten()

        #new a figure and set it into 3d
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        ax.set_title("3D_curve")
        ax.set_xlabel("x")
        ax.set_ylabel("y")
        ax.set_zlabel("z")
        '''ax.plot(x_i[:margin], y_i[:margin], z_i[:margin], c='r')
        ax.plot(x_i[margin:2*margin], y_i[margin:2*margin], z_i[margin:2*margin], c='g')
        ax.plot(x_i[2*margin:-1], y_i[2*margin:-1], z_i[2*margin:-1], c='b')'''
        ax.scatter(x_i[:margin], y_i[:margin], z_i[:margin], c='r')
        ax.scatter(x_i[margin:2 * margin], y_i[margin:2 * margin], z_i[margin:2 * margin], c='g')
        ax.scatter(x_i[2 * margin:-1], y_i[2 * margin:-1], z_i[2 * margin:-1], c='b')
        result_img = result_path + '/class_' + class_n[i] + '_' + str(i)
        plt.savefig(result_img)
        print('i = %d' % (i))
        plt.show()
        plt.clf()
        plt.close()


    print('Done!')
