//
// Created by Marc-Antoine on 2020/2/17.
//


#include "SerialPort.h"

#include <stdio.h>
#include <string.h>

#include <WinSock2.h>
#include <windows.h>
#include<iostream>

#define DATA_SIZE  240//todo 注意匹配

using namespace std;




union {
    char ch[DATA_SIZE*6* sizeof(float)];
    float fl[DATA_SIZE*6];
}rec_union;

SerialPort::SerialPort()
{

}

SerialPort::~SerialPort()
{

}

bool SerialPort::open(const char* portname,
                      int baudrate,
                      char parity,
                      char databit,
                      char stopbit
                      )
{
    HANDLE hCom = nullptr;


        //异步方式
        hCom = CreateFileA(portname, //串口名
                           GENERIC_READ | GENERIC_WRITE, //支持读写
                           0, //独占方式，串口不支持共享
                           NULL,//安全属性指针，默认值为NULL
                           OPEN_EXISTING, //打开现有的串口文件
                           FILE_FLAG_OVERLAPPED, //0：同步方式，FILE_FLAG_OVERLAPPED：异步方式
                           NULL);//用于复制文件句柄，默认值为NULL，对串口而言该参数必须置为NULL


    if(hCom == (HANDLE)-1)
    {
        return false;
    }

    //配置缓冲区大小
    if(! SetupComm(hCom,1024, 1024))
    {
        return false;
    }

    // 配置参数
    DCB p;
    memset(&p, 0, sizeof(p));
    p.DCBlength = sizeof(p);
    p.BaudRate = baudrate; // 波特率
    p.ByteSize = databit; // 数据位

    switch (parity) //校验位
    {
        case 0:
            p.Parity = NOPARITY; //无校验
            break;
        case 1:
            p.Parity = ODDPARITY; //奇校验
            break;
        case 2:
            p.Parity = EVENPARITY; //偶校验
            break;
        case 3:
            p.Parity = MARKPARITY; //标记校验
            break;
    }

    switch(stopbit) //停止位
    {
        case 1:
            p.StopBits = ONESTOPBIT; //1位停止位
            break;
        case 2:
            p.StopBits = TWOSTOPBITS; //2位停止位
            break;
        case 3:
            p.StopBits = ONE5STOPBITS; //1.5位停止位
            break;
    }

    if(! SetCommState(hCom, &p))
    {
        // 设置参数失败
        return false;
    }

    //超时处理,单位：毫秒
    //总超时＝时间系数×读或写的字符数＋时间常量
    COMMTIMEOUTS TimeOuts;
    TimeOuts.ReadIntervalTimeout = 1000; //读间隔超时
    TimeOuts.ReadTotalTimeoutMultiplier = 500; //读时间系数
    TimeOuts.ReadTotalTimeoutConstant = 5000; //读时间常量
    TimeOuts.WriteTotalTimeoutMultiplier = 500; // 写时间系数
    TimeOuts.WriteTotalTimeoutConstant = 2000; //写时间常量
    SetCommTimeouts(hCom,&TimeOuts);

    PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);//清空串口缓冲区

    memcpy(pHandle, &hCom, sizeof(hCom));// 保存句柄

    return true;
}

void SerialPort::close()
{
    HANDLE hCom = *(HANDLE*)pHandle;
    CloseHandle(hCom);
}

int SerialPort::send(const string& dat)
{
    HANDLE hCom = *(HANDLE*)pHandle;

        //异步方式
        DWORD dwBytesWrite = dat.length(); //成功写入的数据字节数
        DWORD dwErrorFlags; //错误标志
        COMSTAT comStat; //通讯状态
        OVERLAPPED m_osWrite; //异步输入输出结构体

        //创建一个用于OVERLAPPED的事件处理，不会真正用到，但系统要求这么做
        memset(&m_osWrite, 0, sizeof(m_osWrite));
        m_osWrite.hEvent = CreateEvent(nullptr, TRUE, FALSE, reinterpret_cast<LPCSTR>(L"WriteEvent"));

        ClearCommError(hCom, &dwErrorFlags, &comStat); //清除通讯错误，获得设备当前状态
        BOOL bWriteStat = WriteFile(hCom, //串口句柄
                                    (char*)dat.c_str(), //数据首地址
                                    dwBytesWrite, //要发送的数据字节数
                                    &dwBytesWrite, //DWORD*，用来接收返回成功发送的数据字节数
                                    &m_osWrite); //NULL为同步发送，OVERLAPPED*为异步发送
        if (!bWriteStat)
        {
            if (GetLastError() == ERROR_IO_PENDING) //如果串口正在写入
            {
                WaitForSingleObject(m_osWrite.hEvent, 1000); //等待写入事件1秒钟
            }
            else
            {
                ClearCommError(hCom, &dwErrorFlags, &comStat); //清除通讯错误
                CloseHandle(m_osWrite.hEvent); //关闭并释放hEvent内存
                return 0;
            }
        }
        return dwBytesWrite;

}

char SerialPort::receive(float rec_float[6][400], float rec_angle[36])
{
    cout<<"receiving."<<endl;
//    union transform rec_union{};
    char buf[6*400*4+8];

//    char buf[100];
    cout<<"initializing buf."<<endl;
    for(char & k : buf)
        k = char(-1);
    cout<<"buf initialized."<<endl;
//    Sleep(500);// 等待蓝牙传入数据
    HANDLE hCom = *(HANDLE*)pHandle;
    DWORD wCount = 6*400* sizeof(float)+8; //成功读取的数据字节数
//    DWORD wCount = 100; //成功读取的数据字节数
    DWORD dwErrorFlags; //错误标志
    COMSTAT comStat; //通讯状态
    OVERLAPPED m_osRead; //异步输入输出结构体


    //创建一个用于OVERLAPPED的事件处理
//    cout<<"placeholder"<<endl;
//    this->send("flush");
    memset(&m_osRead, 0, sizeof(m_osRead));
    m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, reinterpret_cast<LPCSTR>(L"ReadEvent"));

//    cout<<"placeholder"<<endl;
    ClearCommError(hCom, &dwErrorFlags, &comStat); //清除通讯错误，获得设备当前状态
//    cout<<"placeholder"<<endl;

    //如果输入缓冲区字节数为0，则返回fals      e
    BOOL bReadStat = ReadFile(hCom, //串口句柄
                                      buf, //数据首地址
                                      wCount, //要读取的数据最大字节数
                                      &wCount, //DWORD*,用来接收返回成功读取的数据字节数
                                      &m_osRead); //NULL为同步发送，OVERLAPPED*为异步发送if (!bReadStat)
    if (GetLastError() == ERROR_IO_PENDING) //如果串口正在读取中
        {
            //GetOverlappedResult函数的最后一个参数设为TRUE
            //函数会一直等待，直到读操作完成或由于错误而返回
            GetOverlappedResult(hCom, &m_osRead, &wCount, TRUE);
        }
    else
        {
            ClearCommError(hCom, &dwErrorFlags, &comStat); //清除通讯错误
            CloseHandle(m_osRead.hEvent); //关闭并释放hEvent的内存
            return '0';
        }

    cout<<"received."<<endl;
    cout<<"parsing."<<endl;
    //todo 这里是将所有的糅合在了一起
    //在A按钮时，发送命令A便紧接着发送角度，平时状态也是
    //在B按钮时，发送来命令B后先挂机两秒等待手势，此时zens_host收不到任何数据，应该执行默认的'P'情况
    switch (buf[0]){
        //判断是否传来按钮命令
        case 'A':
            for (int i = 0; i < 36; ++i) {
                for (int j = 0; j < 4; ++j) {
                    rec_union.ch[i*4+j] = buf[i*4+j+1];
                }//copy一个float
                rec_angle[i] = rec_union.fl[i];
            }
            return 'A';
        case 'B'://注意啥时候不返回B了啥时候数据接受完，啥时候才能用rec_angle,buf[0]==B, buf[1]==fois
            for(int i=0; i<DATA_SIZE; ++i){
                int temp = int(buf[1])*DATA_SIZE + i;
//                if(int(buf[1]) == 100){
//                    temp = 18*DATA_SIZE + i;//只要把buf[1]=18就崩溃，艹！这是为啥！！？？
//                }else{
//                    temp = int(buf[1])*DATA_SIZE + i;
//                }
                for(int j=0;j<4; ++j){
        //                    rec_union.ch[int(buf[1])*DATA_SIZE*4 + i*4+j] = buf[i*4+j+2];
                    rec_union.ch[i*4+j] = buf[i*4+j+2];
                }
                rec_float[temp/400][temp%400] = rec_union.fl[i];
//                cout<<"c= "<<temp/400<<" r="<<temp%400<<" d="<<rec_float[temp/400][temp%400]<<endl;
            }
            cout<<"batch-"<<int(buf[1])<<endl;

//        {
//            int temp = 0;
//            if(int(buf[1]) == 100){
//                temp = 18;
//            }else{
//                temp = int(buf[1]);
//            }
//            for (int k = 0; k < DATA_SIZE*4; ++k) {
//                rec_union.ch[temp*DATA_SIZE*4 + k] = buf[k+2];
//                if(k%4==0){
//                    rec_float[(temp*DATA_SIZE + k/4)/400][(temp*DATA_SIZE + k/4)%400] = rec_union.fl[temp*DATA_SIZE + k/4];
//                }
//            }
//            for (int l = 0; l < DATA_SIZE; ++l) {
//                rec_float[(temp*DATA_SIZE + l)/400][(temp*DATA_SIZE + l)%400] = rec_union.fl[temp*DATA_SIZE + l];
//            }
//            cout<<"batch-"<<int(buf[1])<<endl;
//        }

            return 'B';
        case 'C':
            return 'C';
        case 'P':
//            if(is_filled(buf)){//如果buf填充有数据的话，用这个数据更新rec_angle
            for (int i = 0; i < 36; ++i) {
                for (int j = 0; j < 4; ++j) {
                    rec_union.ch[i*4+j] = buf[i*4+j+1];
                }//copy一个float
                rec_angle[i] = rec_union.fl[i];
//                cout<<rec_angle[0]<<", "<<rec_angle[1]<<", "<<rec_angle[2]<<endl;
            }
//            } //否则不更新rec_angle，返回上次rec_angle的值
            return 'P';
        case 'T':
            return 'T';
        default:
            return 'P';
    }

}



