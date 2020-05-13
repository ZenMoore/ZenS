# param: 手势类型字符串
# 返回是否发送成功
#  @author Simon
import mmap
import time

def send(gest_type):
    mmap_file = mmap.mmap(-1, 2048, access = mmap.ACCESS_WRITE, tagname = 'shm1')
    lenth=len(gest_type)
    gest_type='r'+gest_type[:lenth]
    mmap_file.seek(0)
    mmap_file.write(gest_type.encode())
    print("gesture sending finished.")
    print(gest_type)
    mmap_file.close()
    return True

# if __name__ == '__main__':
#     while True:
#         str = input("order: ")
#         send(str)