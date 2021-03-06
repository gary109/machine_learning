/*
    Author: GarY 
    Time : 8/17/2017 
    Title : PLA Algorithm 
*/  
#include <stdlib.h>
#include<iostream>  
#include<vector>  
using namespace std;  
#define NUM 9

#define USAGE \
    "\n usage: PLA param=<>...\n"									\
	"\n option parameters:\n"										\
	"    w0=%%f\n"										\
    "    w1=%%f\n"										\
    "    w2=%%f\n"										\
    "    x0=%%f\n"										\
    "    x1=%%f\n"										\
    "    x2=%%f\n"										\
    "\n"																\

/*
* global options
*/


//以二维空间为例，x1 x2为属性  
struct Item{  
    double x0;  
    double x1,x2;  
    int label;  
};  

//权重结构体，w1 w2为属性x1 x2的权重，初始值全设为0  
struct Weight{  
    double w0,w1,w2;//  
}Wit0={0,0,0};  

struct options
{
	Item item;
    Weight wit;
} opt;

struct Item D[NUM] = {
    {1, 1, 1, 1},
    {1, 2, 2, 1},
    {1, 3, 3, 1},
    {1, 4, 4, 1},
    {1, 1, 0.9, -1},
    {1, 2, 1.9, -1},
    {1, 3, 2.9, -1},
    {1, 4, 3.9, -1},
    {1, 4, 3.91, 1},
    // {1, 4, 3.8, 1},
};


  
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

int main(int argc, char* argv[]) {  
    if (argc > 7)
	{
	usage:
		printf(USAGE);
		printf("\n");
		goto exit;
	}
    else if(argc <= 1) { 
        printf("Train machine ...\n");

        vector<Item> ivec;  
        cout<<"Input Item.x1-Item.x2;"<<endl;  

        for(int i=0;i<NUM;i++) {
            printf("[%d] x0=%f, x1=%f, x2=%f, label=%d\n", i, D[i].x0, D[i].x1, D[i].x2, D[i].label);
            ivec.push_back(D[i]);
        } 

        Weight wit=Wit0;  
        unsigned int times = 0;
        for(vector<Item>::iterator iter=ivec.begin();iter!=ivec.end();++iter){  
            times++;
            if((*iter).label!=sign(DotPro(*iter,wit))){  
                wit=UpdateWeight(*iter,wit);  
                iter=ivec.begin();//在从头开始判断，因为更新权重后可能会导致前面的点出故障，需要从头再判断  
            }  
        }  
        
        // print weight  
        printf("times=%d w0=%f, w1=%f, w2=%f\n", times, wit.w0, wit.w1, wit.w2);
    }
    else {
        char *p, *q;
        for (int i = 1; i < argc; i++)
	    {
            p = argv[i];
            if ((q = strchr(p, '=')) == NULL)
                goto usage;
            *q++ = '\0';

            if (strcmp(p, "help") == 0)
                goto usage;
            else if (strcmp(p, "w0") == 0) {
                opt.wit.w0 = atof(q);
            }
            else if (strcmp(p, "w1") == 0) {
                opt.wit.w1 = atof(q);
            }
            else if (strcmp(p, "w2") == 0) {
                opt.wit.w2 = atof(q);
            }
            else if (strcmp(p, "x0") == 0) {
                opt.item.x0 = atof(q);
            }
            else if (strcmp(p, "x1") == 0) {
                opt.item.x1 = atof(q);
            }
            else if (strcmp(p, "x2") == 0) {
                opt.item.x2 = atof(q);
            }
        }

        opt.item.label = sign(DotPro(opt.item, opt.wit));

        // print label  
        printf("label=%d\n", opt.item.label);
    }


exit:
	return 0;
}  