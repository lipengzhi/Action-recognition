'''
This file realize PCA algorithm through sklearn library and step by step.
'''

import numpy as np
from  numpy import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA
from sklearn.utils.extmath import svd_flip
import csv
import plotTemplateData
import sys
from sklearn.preprocessing import scale


# 读取数据
def read_from_dat(path, flag):
    all_data = []
    with open(path) as fp:
    # with open("./dataSet_maskedchirp/query_2.dat") as fp:
        for line in fp.readlines():
            # print(line)
            data = []
            if flag == 0:
                three_dim = line.strip().split(' ')
            else:
                three_dim = line.strip().split(',')
            # data.append(float(line))
            data.append(float(three_dim[1]))
            data.append(float(three_dim[2]))
            data.append(float(three_dim[3]))
            all_data.append(np.array(data))
    return np.array(all_data)


# 减去平均值，归一到中心点
def move_coordinate(data):
    print(data)
    mean_X = data[:, 0].mean()
    mean_Y = data[:, 1].mean()
    mean_Z = data[:, 2].mean()
    length = len(data)
    newdata = np.zeros((length, 3))
    newdata[:, 0] = data[:, 0] - mean_X
    newdata[:, 1] = data[:, 1] - mean_Y
    newdata[:, 2] = data[:, 2] - mean_Z
    print(newdata)
    return newdata


def test_rotation_data(data):
    vec = [1/np.sqrt(3), 1/np.sqrt(3), 1/np.sqrt(3)]  #法向量
    theta = np.pi/6     #旋转角度
    print("法向量为：[", vec[0], vec[1], vec[2], "]")
    print("旋转角度为：", theta)
    ro_mat = np.zeros((3, 3))
    ro_mat[0, 0] = np.cos(theta)+(1-np.cos(theta))*vec[0]*vec[0]
    ro_mat[0, 1] = (1-np.cos(theta))*vec[0]*vec[1]-np.sin(theta)*vec[2]
    ro_mat[0, 2] = (1-np.cos(theta))*vec[0]*vec[2]+np.sin(theta)*vec[1]
    ro_mat[1, 0] = (1-np.cos(theta))*vec[0]*vec[1]+np.sin(theta)*vec[2]
    ro_mat[1, 1] = np.cos(theta)+(1-np.cos(theta))*vec[1]*vec[1]
    ro_mat[1, 2] = (1-np.cos(theta))*vec[1]*vec[2]-np.sin(theta)*vec[0]
    ro_mat[2, 0] = (1-np.cos(theta))*vec[0]*vec[2]-np.sin(theta)*vec[1]
    ro_mat[2, 1] = (1-np.cos(theta))*vec[1]*vec[2]+np.sin(theta)*vec[0]
    ro_mat[2, 2] = np.cos(theta)+(1-np.cos(theta))*vec[2]*vec[2]
    length = len(data)
    test_ro_data = np.zeros((length, 3))
    for i in range(length):
        test_ro_data[i, 0] = data[i, 0] * ro_mat[0, 0] + data[i, 1] * ro_mat[0, 1] + data[i, 2] * ro_mat[0, 2]
        test_ro_data[i, 1] = data[i, 0] * ro_mat[1, 0] + data[i, 1] * ro_mat[1, 1] + data[i, 2] * ro_mat[1, 2]
        test_ro_data[i, 2] = data[i, 0] * ro_mat[2, 0] + data[i, 1] * ro_mat[2, 1] + data[i, 2] * ro_mat[2, 2]
    return test_ro_data


def plot_all_picture(query_c1, query_c2, rst1, rst2):
    fig = plt.figure()
    for i in range(4):
        if i == 0:
            data = query_c1
        if i == 1:
            data = query_c2
        if i == 2:
            data = rst1
        if i == 3:
            data = rst2
        ax = fig.add_subplot(2, 4, i+1, projection='3d')
        ax.set_title("3D scatter")
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.set_zlabel('z')
        length = len(data)
        # figrure1 = ax.plot(dataX[:length//3], dataY[:length//3], dataZ[:length//3], c='r', linewidth=0.8)
        # figrure2 = ax.plot(dataX[length//3-1:length//3*2], dataY[length//3-1:length//3*2], dataZ[length//3-1:length//3*2], c='g', linewidth=0.8)
        # figrure3 = ax.plot(dataX[length//3*2-1:], dataY[length//3*2-1:], dataZ[length//3*2-1:], c='b', linewidth=0.8)
        ax.scatter(data[:length // 3, 0], data[:length // 3, 1], data[:length // 3, 2], c='r', linewidth=0.4)
        ax.scatter(data[length // 3 - 1:length // 3 * 2, 0], data[length // 3 - 1:length // 3 * 2, 1],
                   data[length // 3 - 1:length // 3 * 2, 2], c='g', linewidth=0.4)
        ax.scatter(data[length // 3 * 2 - 1:, 0], data[length // 3 * 2 - 1:, 1], data[length // 3 * 2 - 1:, 2], c='b',
                   linewidth=0.4)
        ax.grid(True)
        x = [data[:, 0].min(), data[:, 0].max()]
        y = [data[:, 1].min(), data[:, 1].max()]
        z = [data[:, 2].min(), data[:, 2].max()]
        t = [0, 0]
        ax.plot(t, t, z, c='k')
        ax.plot(x, t, t, c='k')
        ax.plot(t, y, t, c='k')
    plt.show()


def plot_picture(data):
    # 4张图
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    ax.set_title("3D scatter")
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_zlabel('z')
    length = len(data)
    # figrure1 = ax.plot(dataX[:length//3], dataY[:length//3], dataZ[:length//3], c='r', linewidth=0.8)
    # figrure2 = ax.plot(dataX[length//3-1:length//3*2], dataY[length//3-1:length//3*2], dataZ[length//3-1:length//3*2], c='g', linewidth=0.8)
    # figrure3 = ax.plot(dataX[length//3*2-1:], dataY[length//3*2-1:], dataZ[length//3*2-1:], c='b', linewidth=0.8)
    ax.scatter(data[:length // 3, 0], data[:length // 3, 1], data[:length // 3, 2], c='r', linewidth=0.4)
    ax.scatter(data[length // 3 - 1:length // 3 * 2, 0], data[length // 3 - 1:length // 3 * 2, 1],
                       data[length // 3 - 1:length // 3 * 2, 2], c='g', linewidth=0.4)
    ax.scatter(data[length // 3 * 2 - 1:, 0], data[length // 3 * 2 - 1:, 1], data[length // 3 * 2 - 1:, 2], c='b',
                       linewidth=0.4)
    ax.grid(True)
    x = [data[:, 0].min(), data[:, 0].max()]
    y = [data[:, 1].min(), data[:, 1].max()]
    z = [data[:, 2].min(), data[:, 2].max()]
    t = [0, 0]
    ax.plot(t, t, z, c='k')
    ax.plot(x, t, t, c='k')
    ax.plot(t, y, t, c='k')
    plt.show()

#作者：JxKing
#链接：https://www.jianshu.com/p/4528aaa6dc48
#來源：简书
#简书著作权归作者所有，任何形式的转载都请联系作者获得授权并注明出处。
def pca2(X,k):#k is the components you want
    #mean of each feature
    n_samples, n_features = X.shape
    mean=np.array([np.mean(X[:,i]) for i in range(n_features)])
    #normalization
    norm_X=X-mean
    #scatter matrix
    scatter_matrix=np.dot(np.transpose(norm_X),norm_X)
    print('scatter_matrix: ')
    print(scatter_matrix)
    #Calculate the eigenvectors and eigenvalues
    eig_val, eig_vec = np.linalg.eig(scatter_matrix)
    print('eig_val: ')
    print(eig_val)
    print('eig_vec: ')
    print(eig_vec)
    eig_pairs = [(np.abs(eig_val[i]), eig_vec[:,i]) for i in range(n_features)]
    # sort eig_vec based on eig_val from highest to lowest
    eig_pairs.sort(reverse=True)
    # select the top k eig_vec
    feature=np.array([ele[1] for ele in eig_pairs[:k]])
    #guobo test
    print('feature_pre: ')
    print(feature)
    # for i in range(k):
    #     sum = 0.0
    #     for j in range(k):
    #         sum += feature[i][j]
    #     if sum < 0:
    #         for n in range(k):
    #             feature[i][n] = -feature[i][n]
    # print('feature: ')
    # print(feature)
    #get new data
    data=np.dot(norm_X,np.transpose(feature))
    return data

def index_lst(lst, component=0, rate=0):
    #component: numbers of main factors
    #rate: rate of sum(main factors)/sum(all factors)
    #rate range suggest: (0.8,1)
    #if you choose rate parameter, return index = 0 or less than len(lst)
    if component and rate:
        print('Component and rate must choose only one!')
        sys.exit(0)
    if not component and not rate:
        print('Invalid parameter for numbers of components!')
        sys.exit(0)
    elif component:
        print('Choosing by component, components are %s......'%component)
        return component
    else:
        print('Choosing by rate, rate is %s ......'%rate)
        for i in range(1, len(lst)):
            if sum(lst[:i])/sum(lst) >= rate:
                return i
    return 0

def pca3(X,k):#k is the components you want
    #mean of each feature
    n_samples, n_features = X.shape
    mean = np.array([np.mean(X[:, i]) for i in range(n_features)])
    # normalization
    Mat = X - mean

    # covariance Matrix
    p = n_samples
    n = n_features
    cov_Mat = np.dot(Mat.T, Mat) / (p - 1)

    # PCA by original algorithm
    # eigvalues and eigenvectors of covariance Matrix with eigvalues descending
    U, V = np.linalg.eigh(cov_Mat)

    # Rearrange the eigenvectors and eigenvalues
    U = U[::-1]
    for i in range(n):
        V[i, :] = V[i, :][::-1]
    # choose eigenvalue by component or rate, not both of them euqal to 0
    Index = index_lst(U, component=k)  # choose how many main factors
    if Index:
        v = V[:, :Index]  # subset of Unitary matrix
    else:  # improper rate choice may return Index=0
        print('Invalid rate choice.\nPlease adjust the rate.')
        print('Rate distribute follows:')
        print([sum(U[:i]) / sum(U) for i in range(1, len(U) + 1)])
        sys.exit(0)
    # data transformation
    data = np.dot(Mat, v)
    return data


# def pca4(dataMat,topN):
#     # 形成样本矩阵，样本中心化
#     meanVals= mean(dataMat,axis=0)
#     # print('meanVals:', meanVals)
#     meanRemoved = dataMat - meanVals
#     print('meanRemoved:',meanRemoved)
#     # 计算样本矩阵的协方差矩阵
#     covMat = cov(meanRemoved,rowvar=0)
#     #  对协方差矩阵进行特征值分解，选取最大的 p 个特征值对应的特征向量组成投影矩阵
#     eigVals,eigVects =  linalg.eig(mat(covMat))
#     eigValInd = argsort(eigVals)
#     # print('zuichu1',eigValInd)
#     eigValInd = eigValInd[:-(topN+1):-1]
#     print('daos:',eigValInd)
#     redEigVects = eigVects[:,eigValInd]
#     # print('deaw',redEigVects)
#     # 对原始样本矩阵进行投影，得到降维后的新样本矩阵
#     lowDDataMat = meanRemoved * redEigVects
#     print('meanRemoved:',meanRemoved)
#     print('redEigVects',redEigVects)
#     # reconMat = (lowDDataMat * redEigVects.T)+meanVals
#     return lowDDataMat

def pca4(dataMat, topNfeat):
    meanVals = mean(dataMat, axis=0)
    meanRemoved = dataMat - meanVals #remove mean
    # meanRemoved = dataMat
    # print('meanRemoved',meanRemoved)
    # covMat = cov(meanRemoved, rowvar=0)
    # eigVals,eigVects = linalg.eig(mat(covMat))

    U, sigma, VT = linalg.svd(meanRemoved,full_matrices = 0)
    U,VT = svd_flip(U, VT)
    print('VT',VT)
    # eigValInd = argsort(sigma)            #sort, sort goes smallest to largest
    # eigValInd = eigValInd[:-(topNfeat+1):-1]  #cut off unwanted dimensions
    # print('U',U)
    # redEigVects = VT.T[:,eigValInd]       #reorganize eig vects largest to smallest
    redEigVects = VT.T
    # print('redEigVects',redEigVects)
    # print('redEigVects',redEigVects)
    # redEigVects2 = U[:eigValInd,]
    # lowDDataMat = meanRemoved * redEigVects     #transform data into new dimensions
    lowDDataMat = np.dot(meanRemoved,redEigVects)
    # reconMat = (lowDDataMat * redEigVects.T) + meanVals
    return lowDDataMat




if __name__ == "__main__":

    # path_query1 = "/Users/lipengzhi/Desktop/pca_demo/0.csv"
    # path_query2 = "/Users/lipengzhi/Desktop/pca_demo/0.csv"

    # path_query1 = "/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/templates/0.csv"
    # path_query2 = "/Users/lipengzhi/Desktop/braceletgesturerecognition/tracking/templates/0.csv"

    # (query_c1, zscale_data) = plotTemplateData.read_from_preProcessed_csv(path_query1)
    # (query_c2, zscale_data) = plotTemplateData.read_from_preProcessed_csv(path_query2)

    my_matrix = np.loadtxt(open("/Users/lipengzhi/Desktop/pca_demo/121_1.csv", "rb"), delimiter=",", skiprows=0)


    print('1234:',my_matrix)
    # moved_query = move_coordinate(query_c1)
    # plot_picture(moved_query)
    # test_ro_query = test_rotation_data(moved_query)
    # plot_picture(test_ro_query)
    pca = PCA(n_components=3)
    i =3
    if i == 0:
        pca.fit(my_matrix)
        # print('pca.explained_variance_: ')
        # print(pca.explained_variance_)
        # print('pca.singular_values_: ')
        # print(pca.singular_values_)
        # print('jieguo:',pca.n_components_)
        rst1 = pca.fit_transform(my_matrix)
        print('返回1',rst1)
        plot_picture(rst1)

        # pca.fit(test_ro_query)
        # print('pca.explained_variance_2: ')
        # print(pca.explained_variance_)
        # print('pca.singular_values_2: ')
        # print(pca.singular_values_)
        # rst2 = pca.fit_transform(test_ro_query)
        # print('返回2',rst2)
        # plot_picture(rst2)
    elif i == 1:
        rst1 = pca2(my_matrix, 3)
        print("返回代data：",rst1)
        plot_picture(rst1)

        # rst2 = pca2(my_matrix, 3)
        # print('返回2：',rst2)
        # plot_picture(rst2)
    elif i == 2:
        rst1 = pca3(my_matrix, 3)
        rst2 = pca3(my_matrix, 3)

    elif i == 3:
        rst1 = pca4(my_matrix,3)

        print('11:',rst1)
        plot_picture(rst1)

        # rst2 = pca4(test_ro_query, 3)

    # plot_all_picture(query_c1, query_c2, rst1, rst2)

    #save csv.
    # csvfile = open('/Users/guobo/smart_bracelet/Principal-component-analysis/3d-org.csv', 'w')
    # writer = csv.writer(csvfile)
    # tmp = np.around(moved_query, decimals=6)
    # writer.writerows(tmp)
    #
    # csvfile = open('/Users/guobo/smart_bracelet/Principal-component-analysis/3d-rotation.csv', 'w')
    # writer = csv.writer(csvfile)
    # tmp = np.around(test_ro_query, decimals=6)
    # writer.writerows(tmp)
    #
    # csvfile = open('/Users/guobo/smart_bracelet/Principal-component-analysis/3d-norm.csv', 'w')
    # writer = csv.writer(csvfile)
    # tmp = np.around(pca_test_ro_query, decimals=6)
    # writer.writerows(tmp)


