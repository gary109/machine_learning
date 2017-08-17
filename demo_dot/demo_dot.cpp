#include <iostream>
using namespace std;

/* 示範向量內積  (demo-dot.c) */
/* 
                   (9)
                   (7)
    (2 4 6 8 10) . (5) = 110
                   (3)
                   (1)
*/
void demo_dot() {
#define N 5
	int x[N] = { 2,4,6,8,10 }, y[N] = { 9,7,5,3,1 };
	int i, prod;

	for (i = 0, prod = 0; i<N; ++i)
		prod = prod + x[i] * y[i];
	printf("%d\n", prod);
}
int main() {
    demo_dot();
    return 0;
}