import preprocessing.core_preprocess as preprocess
import os
# import application.receive_from_host as receiver
# import application.receive_from_xlsx as receiver
import application.reveive_from_tsv as receiver
import numpy as np
import application.inference as inf
import application.send_to_demo as messenger

# while(not os.path.exists(DATA_FILE)):
#     pass
#
# with open(DATA_FILE, mode= 'r'):
#     preprocess.run(DATA_FILE)
#

model_path = "./model/gr_model.ckpt-26001"

type_map = {0 : 'croix',
                                   1 : 'down_to_up',
                                   2 : 'right_to_left',
                                   3 : 'round',
                                   4 : 'thunder',
                                   5 : 'triangle',
                                   6 : 'turn'}

# todo @author Louise: 将传入的 char[] raw 转换为 float[] 再转换为 float[6][256] 最后转换为 numpy
# 返回 -1 表示解码失败
# def decode(raw):
#     return np.zeros(shape= [6, 256], dtype= np.float)


if __name__ == '__main__':
    # 从 C-Python 通信中获取[6, 256]数据并解码
    # data = decode(reveiver.get_mmap_info())

    # 从 .xlsx 文件中获取[6, 256]numpy数据
    # data = receiver.get_info()

    while True:
        if os.path.exists('./launcher.temp'):
            # 从 .tsv 文件中获取[6, 256]numpy数据
            data = receiver.get_info()

            # 数据预处理
            data = preprocess.run_without_file(data)

        # if data == -1:
        #     print("decode failed.")
        # else:
        #     if messenger.send(type_map.get(inf.run(data, model_path))):
        #         print("Gesture type is sent to Unity3D.")
        #     else:
        #         print("Gesture type send failed.")
            messenger.send(type_map.get(inf.run(data, model_path)))
            os.remove('./launcher.temp')