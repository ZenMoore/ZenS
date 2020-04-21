//
// Created by ZenMoore on 2020/2/15.
//
#ifndef A_TO_DEMO_H
#define A_TO_DEMO_H

#include "iostream"
using namespace std;

extern float rec_angle[36];

extern float angle0[3];
float A_angle_raw[36] = {0};
float A_angle_data[36] = {0};

// 向Unity3D程序发送'A'作为指令
// C-C# 进程间通信
// @author Simon
bool send_A_order(char *pBuf){
    pBuf[1]='A';
    cout<<"A order sent."<<endl;
    return true;
}

// 从蓝牙中获取三个角度数据
// 蓝牙传来的数据什么格式？怎么解析成角度数组？
// @author Marc-Antoine
bool getA_from_bluetooth(){
    for(int i=0; i<36; i++){
        A_angle_raw[i] = rec_angle[i];
    }
    return true;
}

//将角度归一化到[0, 180]
bool decodeA_data(){
    for(int i = 0; i < 36; i++){
        float deviation = A_angle_raw[i] - angle0[i%3];
        A_angle_data[i] = deviation < -10.0 ? 180.0 + deviation + angle0[i] : deviation;
        A_angle_data[i] = A_angle_data[i] - 90;  //todo 这里是一个产生负值的映射
    }
    return true;
}

//  将角度信息传递到demo中, C-C#通信
// @author Simon
void sendA_to_demo(int x,char *pBuf){
    for(int i=0;i<3;i++){
        int a[2];
        a[1]=A_angle_data[x+i];
        if(a[1]>=0){
            a[0]=1;
            if(a[1]>120)
                a[1]=120;
        }
        else{
            a[0]=2,a[1]=-a[1];
            if(a[1]<-120)
                a[1]=120;
        }
        pBuf[2+i*2]=a[0],pBuf[2+i*2+1]=a[1];
    }
    cout<<"A sent to demo."<< A_angle_data[x]<<", "<<A_angle_data[x+1]<<", "<<A_angle_data[x+2]<<endl;
}

void initializeA(){
    for(int i = 0; i < 36; i++) {
        A_angle_raw[i] = 0;
        A_angle_data[i] = 0;
    }
}

// 1. 初始化
// 2. 发送'A'作为'按钮A指令'至 demo
// 3. 获取后续三个角度数据并预处理
// 4. 将三个处理后角度数据传出至 demo
void run_A_to_demo(char *pBuf){
    initializeA();
    // 用二维数组存储data数据[6, 256]
    if(getA_from_bluetooth()){
        if(decodeA_data()){
            int i=0;
            while(1) if(pBuf[0]=='w'){
                    send_A_order(pBuf);
                    sendA_to_demo(i,pBuf);
                    pBuf[0]='r';
                    cout<<"A to demo finish"<<endl;
                    i+=3;
                    if(i>=36)
                        break;
                }
        }else{
            cout << "decoding angle failed." << endl;
        }
    }else{
        cout << "data not gotten from bluetooth." << endl;
    }
    return;
}
#endif