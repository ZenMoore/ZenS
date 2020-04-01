import mmap
mmap_file = None
##import struct


# @author Marc-Antoine

info_str=[]


# 从内存中读取信息
def read_mmap_info():

    global info_str
    global mmap_file
    mmap_file.seek(0)


    # 把二进制转换为字符串
    info_str = mmap_file.read().translate(None, b'\x00').decode()
    if(info_str[0] != "0" ):
        mmap_file.seek(0)
        mmap_file.write("0".encode())

    # 将0写进第一位表示已经读完
    if(info_str[1:4]=="end"):
        return 0
    else:
        return 1


# 不用初始化，因为每次运行 gestRecog 程序时都是运行完结束的，再次启动时候，相当于已经初始化
# 返回获取的数据，列表形式，需要解析
def get_mmap_info():
    global mmap_file
    global info_str
    ##第二个参数1024是设定的内存大小，单位：字节。如果内容较多，可以调大一点
    mmap_file = mmap.mmap(-1, 1024, access = mmap.ACCESS_WRITE, tagname = 'S_ME')
    #读取有效比特数，不包括空比特
    while(read_mmap_info()):
        pass
    return info_str
