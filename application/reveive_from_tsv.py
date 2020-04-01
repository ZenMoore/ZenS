import pandas as pd
import numpy as np

ORIGINAL_PATH = './data_gotten/'
DATA_FILE = 'data.tsv'

SEQUENCE_LEN = 256


# 将(6,?)的numpy转变为(6,256)的numpy
def intercept(data):
    length = data.shape[1]
    proportion = 0.6  # 尾部删除比例或头部补充比例，因为尾部的无效数据比头部多，所以多删除一点
    results = np.zeros(shape=[6, SEQUENCE_LEN])

    # SEQUENCE_LEN = 256, 当数据的行数比256大的时候
    # 首尾删除数据，头部删除若干行，尾部删除若干行
    # 但是头部删除的行数和尾部删除的行数不一致，其服从 proportion=0.6的比例关系
    if length > SEQUENCE_LEN:
        # 分别计算首尾删除的行数
        devia = length - SEQUENCE_LEN
        down_remove = int(devia * proportion)
        up_remove = devia - down_remove

        # 掐头去尾
        for i in range(0, SEQUENCE_LEN):
            for j in range(0, 6):
                results[j][i] = data[j][i+up_remove] # todo test

    # SEQUENCE_LEN = 256, 当数据的行数比256小的时候
    # 首尾补充数据，头部用第一行复制若干行，尾部用最后一行复制若干行
    # 但是尾部补充的行数和头部补充的行数不一致，其服从 proportion=0.6的比例关系
    elif length < SEQUENCE_LEN:
        # 分别计算首尾补充的行数
        devia = SEQUENCE_LEN - length
        up_insert = int(devia * proportion)
        down_insert = devia - up_insert

        # 通过复制第一行补充头部
        for i in range(0, up_insert):# todo test
            for j in range(0, 6):
                results[j][i] = data[j][0]

        for i in range(up_insert, length+up_insert):# todo test
            for j in range(0, 6):
                results[j][i] = data[j][i]

        # 通过复制最后一行补充尾部
        for i in range(length+up_insert, SEQUENCE_LEN):# todo test
            for j in range(0, 6):
                results[j][i] = data[j][-1]
    else:
        for i in range(0, SEQUENCE_LEN):
            for j in range(0, 6):
                results[j][i] = data[j][i]

    return results


# 将(6, ?)的tsv文件读取为(6,?)的numpy
def convert_to_numpy(path):

    df = pd.DataFrame(pd.read_csv(path, sep='\t', header=None))
    length = df.shape[0] - 1 # 去掉最后一行数据，因为该行数据可能是无效数据

    data = np.zeros(shape=[6, length])

    for i in range(0,length):
        for j in range(0, 6):
            data[j][i] = df.loc[i, j]

    return data


# # 删除tsv最后一行
# def clear_invalid():
#     return None


def get_info():
    data = convert_to_numpy(ORIGINAL_PATH+DATA_FILE)
    data = intercept(data)
    return data

# if __name__ == '__main__':
#
#     data = get_info()
#
#     for i in range(0, SEQUENCE_LEN):
#         for j in range(0, 6):
#             print(data[j][i], end=", ")
#         print()
#     print(data.shape)
