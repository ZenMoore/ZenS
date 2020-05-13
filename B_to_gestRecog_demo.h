//
// Created by ZenMoore on 2020/2/15.
//
#ifndef B_TO_GESTRECOG_DEMO_H
#define B_TO_GESTRECOG_DEMO_H

#include "iostream"
#include <windows.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <time.h>
#include <sstream>

using namespace std;

#define DATA_RAW_MAX_TIME 400
//#define DATA_TOTAL_TIME 256

#define DBL 0.01

extern float rec_float[6][400];

float raw[6][DATA_RAW_MAX_TIME] = {0};
//float data[6][DATA_TOTAL_TIME] = {0};
int valid_length = DATA_RAW_MAX_TIME; //表示数据的有效长度
string filedir = "D:/ZenS/application/data_gotten/data1.tsv";
const char* messagedir = "D:/ZenS/application/launcher.temp";
extern int pos; //todo dataset

// 向Unity3D程序发送‘B’作为指令
// C-C# 进程间通信
// @author Simon
bool send_B_order(char *pBuf){
    pBuf[0]='r';
    pBuf[1]='B';
    cout<<"B sent to gestRecog."<<endl;
    return true;
}

// 从蓝牙中获取[6, ?]未经过截取的数据存放到[6, 400]数组 raw (全局变量)中
// 默认最大存储预留量为400
// 蓝牙传来的数据什么格式？怎么解析成[6, 400]数组？
// @author Marc-Antoine
bool getB_from_bluetooth(){
    for(int i=0; i<6; i++){
        for(int j=0; j<DATA_RAW_MAX_TIME; j++){
            raw[i][j] = rec_float[i][j];
//            cout<<"column="<<i<<"  row="<<j<<"  data="<<raw[i][j]<<endl;
        }
    }
    return true;
}

// 获取数据有效长度
// @author Noah
 bool decodeB_data(){

    //计算[6, 400]中的有数据长度
    //当一个6-tuple全部为零则没有数据
    // valid_length最终是有数据的tuple的数量
//    int valid_length = DATA_RAW_MAX_TIME;
    bool first = true;
    int count = 0;
    int j = 0;
    for(j = 0; j< DATA_RAW_MAX_TIME; j++){

        for(int i = 0; i < 6 ; i++){
            if(raw[i][j] <= DBL){
                count++;
            }
        }
        if(count == 6){

            if(first){
                first = false;
                count = 0;
                continue;
            }else{
                break;
            }
        }else{
            first = true;
            count = 0;
        }

    }
    valid_length = j-1;//如果检测到空tuple，为了防止特殊情况，再往后检查一个，如果仍然为空，就返回有效长度
    cout<<"valid length = "<<valid_length<<endl;

//    // 截取256数据
//    if(valid_length <= DATA_TOTAL_TIME){
//        for (int i = 0; i < valid_length; ++i) {
//            for (int j = 0; j < 6; ++j) {
//                data[j][i] = raw[j][i];
//            }
//        }
//    }
//    else {
//        int q = valid_length / DATA_TOTAL_TIME;
//        for (int i = 0; i < valid_length; ++i) {
//            for (int j = 0; j < 6; ++j) {
//                data[j][i * q] = raw[j][i * q];
//            }
//        }
//    }

//    for(int i=0; i<6; i++){
//        for(int j=0; j<valid_length; j++){
//            data[i][j] = raw[i][j];
//        }
//    }

    return true;
}


// 共享内存方法
// 将data数组发到python-receiver, 发送成功回true
// @author Marc_Antoine
// 无法判断操作是否成功
// 文件方法
// 将(6, ?)数据传送给python作为一个tsv文件当中
//  @author Louise
// @author Noah (把data数组写成excel文件，运行GestRecog程序即可)
bool sendB_to_gestRecog(){
    /*
     * 共享内存方法

//    // 定义共享内存
//    // squeeze data
//    // todo decode in python: 将转成的 char 数组在 Python 中转回为 float 数组
//    char szBuffer[6*DATA_TOTAL_TIME* sizeof(float)/ sizeof(char)];
//    // todo 将 float 二维数组 data 转成 char 数组 szBuffer
//    char* a;
//
//    // 创建共享文件句柄
//    HANDLE hMapFile = CreateFileMapping(
//            INVALID_HANDLE_VALUE,   // 物理文件句柄
//            NULL,   // 默认安全级别
//            PAGE_READWRITE,   // 可读可写
//            0,   // 高位文件大小
//            BUF_SIZE,   // 低位文件大小
//            "S_ME"   // 共享内存名称
//    );
//
//    // 映射缓存区视图 , 得到指向共享内存的指针
//    LPVOID lpBase = MapViewOfFile(
//            hMapFile,            // 共享内存的句柄
//            FILE_MAP_ALL_ACCESS, // 可读写许可
//            0,
//            0,
//            BUF_SIZE
//    );
//
//    a = (char*)lpBase;
//    // 将数据拷贝到共享内存
//    //strcpy((UINT32*)lpBase, szBuffer);
//    *a = '0';
//
//    while(true){
//        if(*a == '0'){
//            cin.getline(&szBuffer[1], sizeof(szBuffer)- sizeof(char));
//            szBuffer[0]='1';
//            memcpy((char *)lpBase, szBuffer,sizeof(szBuffer));
//            if(strcmp(&szBuffer[1], "end") == 0){
//                Sleep(1000);
//                break;
//            }
//            for (char & i : szBuffer) {
//                i='0';
//            }
//            Sleep(100);
//        }
    }

//    // 解除文件映射
//    UnmapViewOfFile(lpBase);
//    // 关闭内存映射文件对象句柄
//    CloseHandle(hMapFile);
    */

    /**
     * 文件方法
     */


    //todo dataset
    string s1 = "D:/ZenS/application/data_gotten/data";
    string s3 = ".tsv";
    ostringstream oss;
    oss << s1 << pos << s3;
    cout<<pos<<endl;
    ofstream out(oss.str());
//    dataset end.

    //todo workflow
//    ofstream out(filedir);
    //workflow end


    if(out.fail()){
        cout<<"error\n";
    }

    for(int i=0; i<valid_length; i++){
        out<<raw[0][i]<<"\t"<<raw[1][i]<<"\t"<<raw[2][i]<<"\t"<<raw[3][i]<<"\t"<<raw[4][i]<<"\t"<<raw[5][i]<<endl;
    }
    out.close();
    cout<<"data sent to gestRecog."<<endl;

    //执行gestRecog程序
//    system(exedir);
    ofstream launcher(messagedir);
    launcher<<'1';
    cout<<"GestRecog launched."<<endl;
    return true;
}


void initializeB(){
    // zeroize
    for(int i = 0; i < 6; i++) {
        for (int j = 0; j < DATA_RAW_MAX_TIME; j++) {
            raw[i][j] = 0;
//            if(j < DATA_TOTAL_TIME) data[i][j] = 0;
        }
    }
}

// 1. 初始化 raw 和 data
// 2. 发送'B'作为'按钮B指令'至demo
// 3. 解析后续[6, ?]数据数组，并同时进行数据截取
// 4. 将数组传出至 gestRecog/application/receive_from_host
void run_B_to_gestRecog_demo(char *pBuf){
    initializeB();
//    for(int i=0; i<6; i++){
//        for(int j=0; j<DATA_RAW_MAX_TIME; j++){
//            cout<<"column="<<i<<"  row="<<j<<"  data="<<rec_float[i][j]<<endl;
//        }
//    }
    // 用二维数组存储data数据[6, 256]
    if(getB_from_bluetooth()){
        if(decodeB_data()){
            sendB_to_gestRecog();
//            while(1){//todo
//                if(pBuf[0]=='w'){
//                Sleep(8000);
                    send_B_order(pBuf);
//                    break;
//                }
//            }

//            sendB_to_gestRecog();
        }else{
            cout << "decoding gest_data failed." << endl;
        }
    }else{
        cout << "data not gotten from bluetooth." << endl;
    }
    return;
    }

#endif