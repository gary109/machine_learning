/*
    Author: DreamerMonkey 
    Time : 8/17/2017 
    Title : Pocket PLA Algorithm 
*/  
#include<iostream>  
#include<vector>  
using namespace std;  
#define NUM 6

//以二维空间为例，x1 x2为属性  
struct Item{  
    int x0;  
    double x1,x2;  
    int label;  
};  

// x0, x1, x2, label
struct Item D[NUM] = {
    {1, 0.234, 4.95, 1},
    {1, 71.34, 7.56, -1},
    {1, 1.54, 1.5, 1},
    {1, 2.26, 4.5, 1},
    {1, 3.123, 3.5, 1},
    {1, 7.234, 34.445, -1}};

//权重结构体，w1 w2为属性x1 x2的权重，初始值全设为0  
struct Weight{  
    double w0,w1,w2;//  
}Wit0={0,0,0};  
  
//符号函数，根据向量内积和的特点判断是否应该发放信用卡  
int sign(double x){  
    if(x>0)  
        return 1;  
    else if(x<0)  
        return -1;  
    else return 0;  
}  
//两个向量的内积  
double DotPro(Item item,Weight wight){  
    return item.x0*wight.w0+item.x1*wight.w1+item.x2*wight.w2;  
}  
//更新权重  
Weight UpdateWeight(Item item,Weight weight){  
    Weight newWeight;  
    newWeight.w0=weight.w0+item.x0*item.label;  
    newWeight.w1=weight.w1+item.x1*item.label;  
    newWeight.w2=weight.w2+item.x2*item.label;  
    return newWeight;  
}  
int main(){  
    vector<Item> ivec;  
    cout<<"Input Item.x1-Item.x2;"<<endl;  

    for(int i=0;i<NUM;i++) {
        printf("[i] x0=%d, x1=%f, x2=%f, label=%d\n", D[i].x0, D[i].x1, D[i].x2, D[i].label);
        ivec.push_back(D[i]);
    } 

    Weight wit=Wit0;  
    for(vector<Item>::iterator iter=ivec.begin();iter!=ivec.end();++iter){  
        if((*iter).label!=sign(DotPro(*iter,wit))){  
            wit=UpdateWeight(*iter,wit);  
            iter=ivec.begin();//在从头开始判断，因为更新权重后可能会导致前面的点出故障，需要从头再判断  
        }  
    }  
    //打印结果  
    printf("w0=%f, w1=%f, w2=%f\n", wit.w0, wit.w1, wit.w2);
}  