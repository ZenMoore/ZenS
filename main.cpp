//
// Created by ZenMoore on 2020/2/15.
//
#include "none_to_demo.h"
#include "A_to_demo.h"
#include "B_to_gestRecog_demo.h"
#include "iostream"
#include <windows.h>
#include "SerialPort.h"

#define BUF_SIZE 1025

char szName[] = "shm";
char *pBuf;
HANDLE hMapFile;

SerialPort w;
float rec_float[6][400];
float rec_angle[36]={0};
char order = 'P'; //用于存储接受到的命令
int pos = 0;//todo dataset

// 从这里获取单片机传来的按钮信息
// 如果有按按钮的话，单片机传来的数据以一个字符char打头，分别为'A', 'B'
// 此外，还有可能只收到一个字符 'H', 表示此时正在做手势或者校准，程序挂起(在main函数已经写好了)
// 如果没有按按钮，单片机只传三轴角度数据，没有char打头
// 事实上，单片机通过按下按钮的信息选择发送那种数据
// 这里再获取一次按钮信息，一是指令匹配，另一个是控制分流
// 默认状态没有返回按钮信息，此时返回-1
// 如果检测到char打头，将这个char返回
// @author Marc-Antoine
char get_flow(){
    return w.receive(rec_float,rec_angle); //当这个函数内部对rec_float/rec_angle参数进行改变的时候，作为main全局变量的rec_float/rec_angle也被改变
}

void shm_initial()
{
    hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // �����ļ����
            NULL,                    // Ĭ�ϰ�ȫ����
            PAGE_READWRITE,          // �ɶ���д
            0,                       // ��λ�ļ���С
            BUF_SIZE,                // ��λ�ļ���С
            szName                   // �����ڴ�����
    );
    pBuf = (char *)MapViewOfFile(
            hMapFile,            // �����ڴ�ľ��
            FILE_MAP_ALL_ACCESS, // �ɶ�д���
            0,
            0,
            0
    );
    for(int i=0;i<=1024;i++)
        pBuf[i]=0;
    pBuf[0]='w';
}

// 主程序，循环运行从单片机获取信息流
int main(){
    // 参数initial判断是否为初次启动，如果是，则设置角度angle0以便于计算偏差
    bool initial = true;

    if(w.open("Com9")){
        cout<<"com opening succeeds."<<endl;
        shm_initial();
        while(true){
            order = get_flow();
            cout<<"parsed."<<endl;
            switch(order){
//                case 'A': run_A_to_demo();break;
//                case 'B' : run_B_to_gestRecog_demo(); break;
//                case 'C': Sleep(4000); break;// @author Marc-Antoine
//                case 'P' : run_none_to_demo(initial); initial = false; break;
                case 'P':
                    run_none_to_demo(initial,pBuf);
                    initial = false;
                    break;
                case 'A':
                    run_A_to_demo(pBuf);
                    break;
                case 'B'://注意啥时候不返回B了啥时候数据接受完，啥时候才能用rec_float；buf[0]==B, buf[1]==fois
                    //什么也不做，继续接收数据，直到收到'T'——表征B-data各批次发送完毕的字符
                    break;
                case 'C':
                    initial=true;
                    cout<<"get C"<<endl;
                    Sleep(4000) ;//
                    break;
                case 'T':
                    run_B_to_gestRecog_demo(pBuf);
                    pos++;
                    break;
                    //B的数据传输结束后使用
                default:
                    cout<<"unknwn flow."<<endl;
                    break;
            }
        }
        w.close();
    }
    return 0;
}
