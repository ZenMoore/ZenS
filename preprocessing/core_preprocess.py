import xlrd
import numpy as np
###
import matplotlib.pyplot as plt
###

# 这两个数据是根据JY61上位机得到的数据给出的，而不是根据本项目上位机得到的数据
SIGNAL_AMP = 0.5
FILTER_SIZE = 5

# 给定一个.xls文件，将其转变为(6, 256)维 numpy 向量
def convert_to_numpy(data_file):
    book = xlrd.open_workbook('../application/{}'.format(data_file))

    table = book.sheet_by_index(0)  # 这里是引入第一个sheet，默认是引入第一个，要引入别的可以改那个数字

    # nrows=table.nrows
    # ncols=table.ncols

    start = 1
    end = 257  # 这两个数是为了避开标题行，手动避的
    # rows=start-end

    list_values = []
    for i in range(1, 7):
        values = []
        for x in range(start, end):
            row = table.row_values(x)

            values.append(row[i])
        list_values.append(values)
    # print(list_values)
    data = np.array(list_values)
    return data

# method: convolution
def noise_reduction(data):
    for i in range(6):
        X = data[i]
        wave = np.ones(FILTER_SIZE)*SIGNAL_AMP
        data[i] = np.convolve(X, wave, 'same')
    return data

def zero_mean(data):
    for i in range(6):
        data[i] -= np.mean(data[i], axis=0)
    return data

def normalization(data):
    for i in range(6):
        data[i] /= np.std(data[i], axis=0)
    return data

def whitening(data):
    cov = np.dot(data.T, data) / data.shape[0]  # get the data_gotten covariance matrix
    U, S, V = np.linalg.svd(cov)
    data = np.dot(data, U)  # decorrelate the data_gotten
    return data

# todo 归一化有点问题，白化不知道是否正确(暂时去掉)
def run(data_file):
    data = convert_to_numpy(data_file)
    data = noise_reduction(data)
    data = zero_mean(data)
    data = normalization(data)
    # data = whitening(data)
    return data

def run_without_file(data):
    data = noise_reduction(data)
    data = zero_mean(data)
    data = normalization(data)
    return data