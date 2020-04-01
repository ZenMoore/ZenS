//
// Created by Marc-Antoine on 2020/2/17.
//

#ifndef UNTITLED1_SERIALPORT_H
#define UNTITLED1_SERIALPORT_H
#include <iostream>
using namespace std;

class SerialPort
{
public:
    SerialPort();
    ~SerialPort();

    // 打开串口,成功返回true，失败返回false
    // portname(串口名): 在Windows下是"COM1""COM2"等，在Linux下是"/dev/ttyS1"等
    // baudrate(波特率): 9600、19200、38400、43000、56000、57600、115200
    // parity(校验位): 0为无校验，1为奇校验，2为偶校验，3为标记校验
    // databit(数据位): 4-8，通常为8位
    // stopbit(停止位): 1为1位停止位，2为2位停止位,3为1.5位停止位
    // synchronizable(同步、异步): 0为异步，1为同步
    bool open(const char* portname, int baudrate=38400, char parity=0, char databit=8, char stopbit=1);

    //关闭串口，参数待定
    void close();

    //发送数据或写数据，成功返回发送数据长度，失败返回0
    int send(const string& dat);

    //接受数据或读数据，成功返回读取实际数据的长度，失败返回0
    char receive(float rec_float[6][400], float rec_angle[36]);
    int pHandle[16];
};
#endif //UNTITLED1_SERIALPORT_H
