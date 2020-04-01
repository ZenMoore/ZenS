//
// Created by ZenMoore on 2020/2/15.
//
#ifndef NONE_TO_DEMO_H
#define NONE_TO_DEMO_H

#include "iostream"
using namespace std;


extern float rec_angle[36];
float angle0[3] = {0};
float void_angle_raw[36] = {0};
float void_angle_data[36] = {0};

// 向Unity3D程序发送'P'作为指令
// C-C# 进程间通信
// todo @author Simon
void send_N_order(){
    cout<<"N sent to demo."<<endl;
}

// 从蓝牙中获取三个角度数据, rec_angle这个外部变量存储着从蓝牙获取的信息
// @author Marc-Antoine
bool getN_from_bluetooth(){
    for(int i=0; i<36; i++){
        void_angle_raw[i] = rec_angle[i];
//        cout<<void_angle_raw[i]<<endl;
    }
    return true;
}

//将角度归一化到[0, 180]
// 参数initial判断是否为初次启动，如果是，则设置角度angle0以便于计算偏差
bool decodeN_data(bool initial){
    if(initial){
        for(int i = 0; i < 3; i++){
            angle0[i] = void_angle_raw[i];
        }
    }

    for(int i = 0; i < 36; i++){
//        cout<<angle0[i]<<endl;
        float deviation = void_angle_raw[i] - angle0[i%3];
        void_angle_data[i] = deviation < -10.0 ? 180.0 + deviation + angle0[i] : deviation;
//        cout<< void_angle_data[i]<<endl;
    }
    return true;
}

//  将角度信息传递到demo中, C-C#通信
// todo @author Simon
// todo @author Louise: maybe there shoule be a transfer of float to char[]
void sendN_to_demo(){
    for(int i = 0; i<36; i+=3) {
        cout << "N_order sent." << void_angle_data[i] << ", " << void_angle_data[i + 1] << ", "
             << void_angle_data[i + 2] << endl;
    }
}

void initializeN(){
    for(int i = 0; i < 36; i++){
        void_angle_raw[i] = 0;
        void_angle_data[i] = 0;
    }
}

// 1. 初始化
// 2. 发送 'P' 作为 '结束按钮指令' 至 demo
// 3. 获取后续三个角度数据并预处理
// 4. 将三个处理后角度数据传出至 demo
// 参数initial判断是否为初次启动，如果是，则设置角度angle0以便于计算偏差
void run_none_to_demo(bool initial){
    initializeN();
    send_N_order();

    // 用二维数组存储data数据[6, 256]
    if(getN_from_bluetooth()){
        if(decodeN_data(initial)){
            sendN_to_demo();
        }else{
            cout << "decoding angle failed." << endl;
        }
    }else{
        cout << "data not gotten from bluetooth." << endl;
    }
    return;
}

#endif