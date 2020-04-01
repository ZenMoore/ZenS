import tensorflow as tf
import numpy as np
import training.forward as forward
import xlrd
import os
import itertools
from random import shuffle

NUM_EXAMPLES = forward.OUTPUT_NODE * 36

LEARNING_RATE_BASE = 0.001
# LEARNING_RATE_DECAY = 0.99
# MOVING_AVERAGE_DECAY = 0.99
REGULARIZATION_RATE = 0.0001
TRAIN_STEPS = 50000 # todo use evaluation percentage threshold

MODEL_SAVE_PATH = '../models/'
MODEL_NAME = 'gr_model.ckpt'

DATASET_PATH = '../dataset/new/'

SHUFFLE_BUFFER_SIZE = 80

batch_pos = 0

def convert_to_one_hot(tensor):
    a = tensor
    mydic = set()

    for i in range(NUM_EXAMPLES):
        item = a[i]
        mydic.add(item)
    # print(mydic)

    # 把这八个单个[]合起来,为了合并加了一个全是0的
    c = np.zeros(forward.OUTPUT_NODE)
    for item in mydic:
        b = np.zeros(forward.OUTPUT_NODE)
        b[int(item)] = 1
        c = np.vstack((c, b))
    # print(c)

    # 删去第一个全是0的
    index = [0]  # 最后一个的索引,因为最后一个是0
    c = np.array(list(itertools.compress(c, [i not in index for i in range(len(c))])))
    # print(c)

    # 每个重复36次
    e = np.zeros(forward.OUTPUT_NODE)
    for array in c:
        # print(array)
        d = np.tile(array, (int(NUM_EXAMPLES/forward.OUTPUT_NODE), 1))  # 重复36次
        e = np.vstack((e, d))
    # print(d)

    # 删去第一个空的
    index = [0]  # 最后一个的索引
    e = np.array(list(itertools.compress(e, [i not in index for i in range(len(e))])))
    # print(e)
    return e

# 将存储数据的.xlsx文件转为(6, 256)手势序列numpy矩阵
def convert_to_numpy(data_file):
    book = xlrd.open_workbook(DATASET_PATH+'{}'.format(data_file))
    # book = xlrd.open_workbook(data_file)

    table = book.sheet_by_index(0)  # 这里是引入第一个sheet，默认是引入第一个，要引入别的可以改那个数字

    # nrows=table.nrows
    # ncols=table.ncols

    start = 0
    end = forward.INPUT_NODE_VERTICAL + start  # 这两个数是为了避开标题行，手动避的
    # rows=start-end

    list_values = []
    for i in range(6):
        values = []
        for x in range(start, end):
            row = table.row_values(x)

            values.append(row[i])
        list_values.append(values)
    # print(list_values)
    data = np.array(list_values)
    return data

# 返回的dataset是一个[(data, label), (data, label), (data, label), (data, label)...]
def get_dataset(path):
    datas = np.zeros(shape= [NUM_EXAMPLES, forward.INPUT_NODE_HORIZONTAL, forward.INPUT_NODE_VERTICAL], dtype= np.float32)
    labels = np.zeros(shape= [NUM_EXAMPLES], dtype= np.int32)
    count_label = 0 # 访问到了第几个数据集子文件夹
    count_sequence = 0
    dirs = os.listdir(DATASET_PATH)

    dataset = [] # list of tuples
    for dir in dirs:
        # 赋值 36 个 labels
        labels[count_label * int(NUM_EXAMPLES/forward.OUTPUT_NODE) : (count_label + 1) * int(NUM_EXAMPLES/forward.OUTPUT_NODE)] = np.ones(shape= [int(NUM_EXAMPLES/forward.OUTPUT_NODE)]) * count_label
        count_label += 1
        # 赋值 36 个手势序列
        for file in os.listdir(DATASET_PATH+'{}'.format(dir)):
            datas[count_sequence] = convert_to_numpy(dir+'/'+file)
            count_sequence += 1


    labels = convert_to_one_hot(labels)
    for i in range(NUM_EXAMPLES):
        dataset.append((datas[i], labels[i]))
    shuffle(dataset)
    shuffle(dataset)
    shuffle(dataset) # todo 打乱三次，确保够乱
    return dataset

def next_batch(dataset):
    global batch_pos

    data_list = []
    label_list = []

    for i in range(batch_pos, batch_pos + forward.BATCH_SIZE):
        data_list.append(dataset[i][0])
        label_list.append(dataset[i][1])

    batch_pos += forward.BATCH_SIZE
    if batch_pos >= forward.BATCH_SIZE:
        batch_pos = 0

    assert len(data_list) != 0
    assert len(data_list) != 0

    return data_list, label_list

def train(dataset):
    # iterator = dataset.make_initializable_iterator()

    data = tf.placeholder(dtype= tf.float32, shape= [None, forward.INPUT_NODE_HORIZONTAL, forward.INPUT_NODE_VERTICAL], name= 'data_input')
    label = tf.placeholder(dtype= tf.float32, shape= [None, forward.OUTPUT_NODE], name= 'label_input')

    regularizer = tf.contrib.layers.l2_regularizer(REGULARIZATION_RATE)
    y = forward.inference(data, regularizer)
    global_step = tf.Variable(0, trainable= False)

    cross_entropy = tf.nn.sparse_softmax_cross_entropy_with_logits(logits= y, labels= tf.argmax(label, 1))
    cross_entropy_mean = tf.reduce_mean(cross_entropy)
    loss = cross_entropy_mean + tf.add_n(tf.get_collection('losses'))

    learning_rate = tf.train.cosine_decay_restarts(LEARNING_RATE_BASE, global_step, int(NUM_EXAMPLES/forward.BATCH_SIZE), t_mul= 2.0, m_mul= 1.0)

    # variable_average = tf.train.ExponentialMovingAverage(MOVING_AVERAGE_DECAY, global_step)
    # variable_average_op = variable_average.apply(tf.trainable_variables())

    train_step = tf.train.AdamOptimizer(learning_rate).minimize(loss, global_step= global_step)
    with tf.control_dependencies([train_step]): # 之前有滑动平均，之后去掉了
        train_op = tf.no_op(name= 'train')

    saver = tf.train.Saver()

    with tf.Session(config= tf.ConfigProto(log_device_placement= False, allow_soft_placement=True)) as sess:
        # with tf.device("/gpu:0"):
        tf.global_variables_initializer().run()

        for i in range(TRAIN_STEPS):
            data_feed, label_feed = next_batch(dataset)

            _, loss_value, step = sess.run([train_op, loss, global_step], feed_dict={data: data_feed, label: label_feed})

            if i%1000 == 0:

                print("After %d training step(s), loss on training batch is %g ." % (step, loss_value))
                # 给出global_step参数可以让每个被保存模型的文件名末尾加上训练的轮数，比如model.ckpt=1000表示训练1000论之后的模型
                saver.save(sess, os.path.join(MODEL_SAVE_PATH, MODEL_NAME), global_step=global_step)

def main(argv = None):
    dataset = get_dataset(DATASET_PATH)
    train(dataset)

if __name__ == '__main__':
    tf.app.run()


