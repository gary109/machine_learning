/*
    Author: GarY 
    Time : 8/17/2017 
    Title : Pocket PLA Algorithm 
*/  
#include <stdlib.h>
#include<iostream>  
#include <ctime>
#include<vector>  
using namespace std;  

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define MAX_ROUNDS  10000
#define NUM         10

#define USAGE \
    "\n usage: Pocket PLA param=<>...\n"									\
	"\n option parameters:\n"										\
	"    w0=%%f\n"										\
    "    w1=%%f\n"										\
    "    w2=%%f\n"										\
    "    x0=%%f\n"										\
    "    x1=%%f\n"										\
    "    x2=%%f\n"										\
    "\n"																\

//以二维空间为例，x1 x2为属性  
struct Item{  
    int x0;  
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
    
unsigned int fail_counts = MAX_ROUNDS;
unsigned int fail_counts_perface = MAX_ROUNDS;

// x0, x1, x2, label
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
    {1, 4, 3.8, 1},
};
  
//符号函数，根据向量内積和的特徵判断  
int sign(double x){  
    if(x>0) return 1;  
    else if(x<0) return -1;  
    else return 0;  
}  
//两个向量的内积  
double DotPro(Item item,Weight wight){  
    return item.x0*wight.w0+item.x1*wight.w1+item.x2*wight.w2;  
}  
//更新權重 
Weight UpdateWeight(Item item,Weight weight){  
    Weight newWeight;  
    newWeight.w0=weight.w0+item.x0*item.label;  
    newWeight.w1=weight.w1+item.x1*item.label;  
    newWeight.w2=weight.w2+item.x2*item.label;  
    return newWeight;  
}  

int main(int argc, char* argv[]) {  
    std::srand ( unsigned ( std::time(0) ) );
    if (argc > 7)
	{
	usage:
		printf(USAGE);
		printf("\n");
		goto exit;
	}
    else if(argc <= 1) { 
        vector<Item> ivec;  
        vector<Item> ivec_fail;  
        cout<<"Input Test Datas ..."<<endl;  
        for(int i=0;i<NUM;i++) {
            printf("D[%d] x0=%d, x1=%f, x2=%f, label=%d\n", i, D[i].x0, D[i].x1, D[i].x2, D[i].label);
            ivec.push_back(D[i]);
        } 

        Weight wit = Wit0;  
        Weight wit_perface = {0,0,0};
        
        unsigned int times = 0;
        fail_counts = 0;
        for(times=0;times<MAX_ROUNDS;times++){
            ivec_fail.clear();

            // 找出錯誤的D
            for(vector<Item>::iterator iter=ivec.begin();iter!=ivec.end();++iter){  
                if((*iter).label!=sign(DotPro(*iter,wit))){  
                    ivec_fail.push_back(*iter);
                }  
            }  

            //隨機挑出一個進行修正，更新權重
            if(!ivec_fail.empty()){
                fail_counts = ivec_fail.size();
                unsigned int rand_number = rand() % ivec_fail.size();
                wit = UpdateWeight(ivec_fail[rand_number], wit);
                if(fail_counts < fail_counts_perface) {
                    fail_counts_perface = fail_counts;
                    wit_perface.w0 = wit.w0;
                    wit_perface.w1 = wit.w1;
                    wit_perface.w2 = wit.w2;
                }
            }
            else
                break;
        }

        //打印结果  
        printf("Train %d times min fail=%d w0=%f, w1=%f, w2=%f\n",times, fail_counts_perface, wit_perface.w0, wit_perface.w1, wit_perface.w2);
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