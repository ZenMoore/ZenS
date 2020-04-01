import tensorflow as tf
import numpy as np
slim = tf.contrib.slim

'''
关于所保存的最佳 model 的信息：
    dataset shuffle buffer size= 80
    batch size= 30

    Optimizer= Adam
    Regularizer= L2
    Learning_rate_decay= cosine_decay_restarts
    Initializer_weights= Xavier_initializer
    Initializer_bias= Xavier_initializer
    Initializer of fc= truncated_normal_initializer(mean= 0.0, stddev= 1.0)
    activation_in_cnn_part= tanh
    activation_between= relu
    activation_in_rnn_part= tanh
    merge from 6 column to 1= reshape(splice)
    
    NO: moving_average_op, dropout 
'''

INPUT_NODE_HORIZONTAL = 6
INPUT_NODE_VERTICAL = 256
OUTPUT_NODE = 7

GRU_HIDDEN_SIZE = 256
GRU_NUM_STEPS = 8

input_tensor = tf.placeholder(dtype= tf.float32, shape= [None, INPUT_NODE_HORIZONTAL, INPUT_NODE_VERTICAL], name= 'data_input')


# def get_weight_variable(shape):
#     weights = tf.get_variable('weights', shape= shape, initializer= tf.contrib.layers.variance_scaling_initializer())
#     return weights


# convert (?, 6, 256) to (?, 6, 256, 1)
def add_one_dimension(input_tensor):

    return tf.expand_dims(input_tensor, [-1])

# input_tensor: (?, 6, 256)
# output: (?,8)

biases = tf.get_variable('biases', [48], initializer= tf.contrib.layers.xavier_initializer())

input_tensor_4dims = add_one_dimension(input_tensor)

with tf.variable_scope('layer_conv01'):

    conv01_filter_weight = tf.get_variable('filter_weight', [1, 3, 1, 12], initializer= tf.contrib.layers.xavier_initializer())
    conv01_biases = tf.get_variable('bias', [12], initializer=tf.contrib.layers.xavier_initializer())
    conv01 = tf.nn.conv2d(input_tensor_4dims, conv01_filter_weight, strides= [1, 1, 2, 1], padding='SAME')
    relu01 = tf.nn.tanh(tf.nn.bias_add(conv01,conv01_biases))


# with tf.name_scope('layer_pool01'):
#     pool01 = tf.nn.max_pool(relu01, ksize=[1, 1, 1, 1], strides=[1, 1, 1, 1], padding='VALID')

with tf.variable_scope('layer_conv02'):
    conv02_filter_weight = tf.get_variable('filter_weight', [1, 3, 12, 24], initializer= tf.contrib.layers.xavier_initializer())
    conv02_biases = tf.get_variable('bias', [24], initializer=tf.contrib.layers.xavier_initializer())
    conv02 = tf.nn.conv2d(relu01, conv02_filter_weight, strides= [1, 1, 2, 1], padding='SAME')
    relu02 = tf.nn.tanh(tf.nn.bias_add(conv02,conv02_biases))

# with tf.name_scope('layer_pool02'):
#     pool02 = tf.nn.max_pool(relu02, ksize=[1, 1, 1, 1], strides=[1, 1, 1, 1], padding='VALID')

with tf.variable_scope('layer_conv03'):
    conv03_filter_weight = tf.get_variable('filter_weight', [1, 3, 24, 48], initializer= tf.contrib.layers.xavier_initializer())
    conv03_biases = tf.get_variable('bias', [48], initializer=tf.contrib.layers.xavier_initializer())
    conv03 = tf.nn.conv2d(relu02, conv03_filter_weight, strides= [1, 1, 2, 1], padding='SAME')
    relu03 = tf.nn.tanh(tf.nn.bias_add(conv03,conv03_biases))

# with tf.name_scope('layer_pool03'):
#     pool03 = tf.nn.max_pool(relu03, ksize=[1, 1, 1, 1], strides=[1, 1, 1, 1], padding='VALID')

with tf.variable_scope('layer_conv04'):
    conv04_filter_weight = tf.get_variable('filter_weight', [1, 3, 48, 48], initializer= tf.contrib.layers.xavier_initializer())
    conv04_biases = tf.get_variable('bias', [48], initializer=tf.contrib.layers.xavier_initializer())
    conv04 = tf.nn.conv2d(relu03, conv04_filter_weight, strides= [1, 1, 1, 1], padding='SAME')
    relu04 = tf.nn.tanh(tf.nn.bias_add(conv04, conv04_biases))

# with tf.name_scope('layer_pool04'):
#     pool04 = tf.nn.max_pool(relu04, ksize=[1, 1, 1, 1], strides=[1, 1, 1, 1], padding='VALID')

with tf.variable_scope('conv_to_recur'):
    bn = tf.layers.batch_normalization(relu04, training= False)
    bias = tf.nn.bias_add(bn, biases) # dropout 和 bias 的顺序不知道是不是正确
    actived = tf.nn.relu(bias)

    into_gru = tf.reduce_sum(actived, 3)

with tf.variable_scope('gru_layers'):

    gru = tf.nn.rnn_cell.GRUCell(GRU_HIDDEN_SIZE, activation= tf.nn.tanh, kernel_initializer= tf.contrib.layers.xavier_initializer(), bias_initializer= tf.contrib.layers.xavier_initializer())
    initial_state = gru.zero_state(batch_size= 1, dtype= tf.float32)
    # 使用static_rnn运行num_steps, 但是不能保证遍历全部时间点，可以更换为 dynamic_rnn
    # print(actived[:,0,:].shape)
    # for i in range(GRU_NUM_STEPS):
    #     if i > 0 : tf.get_variable_scope().reuse_variables()
    #     gru_output, state = gru(into_gru[:,i,:], state= state)]
    gru_output, state = tf.nn.dynamic_rnn(gru, into_gru, initial_state=initial_state)

with tf.variable_scope('fully_connected'):
    # trim_dim = tf.reduce_mean(gru_output, axis=1, keep_dims=False)
    trim_dim = tf.reshape(gru_output,
                          shape=[1, INPUT_NODE_HORIZONTAL * GRU_HIDDEN_SIZE])

    fc_weight = tf.get_variable('fc_weight', [INPUT_NODE_HORIZONTAL * GRU_HIDDEN_SIZE, OUTPUT_NODE],
                                initializer=tf.truncated_normal_initializer(mean= 0.0, stddev= 1.0))

    fc_bias = tf.get_variable('bias', [OUTPUT_NODE], initializer=tf.truncated_normal_initializer())

    logits = tf.matmul(trim_dim, fc_weight) + fc_bias
    # fc_weight = tf.get_variable('fc_weight', [GRU_HIDDEN_SIZE, OUTPUT_NODE], initializer= tf.truncated_normal_initializer(stddev= 0.1))
    #
    # fc_bias = tf.get_variable('bias', [OUTPUT_NODE], initializer= tf.constant_initializer(0.1))
    #
    # logits = tf.matmul(gru_output, fc_weight) + fc_bias
    # y = tf.nn.softmax(logits=logits)
    y = tf.nn.softmax(logits= logits)


def run(data, model_path):
    with tf.Session() as sess:
        saver = tf.train.Saver()
        saver.restore(sess, model_path)
        data = [data]
        out = sess.run(y, feed_dict={input_tensor: data})
        result = np.argmax(out)
        return result

