#include <iostream>
using namespace std;
/* 示範矩陣向量外積  (demo-mvmul.c) */
/*  
    (1 2 3 4)   (6)   (53)
    (5 6 7 8) x (5) = (141)
    (9 8 7 6)   (7)   (167)
                (4)
*/
void demo_mvmul() {
#define M 3
#define N 4
	int A[M][N] = { {1,2,3,4},{5,6,7,8},{9,8,7,6}}, x[N] = { 6,5,7,4 }, y[M];
	int i, j;

	for (i = 0; i<M; ++i) {
		y[i] = 0;
		for (j = 0; j<N; ++j)
			y[i] = y[i] + A[i][j] * x[j];
	}
	for (i = 0; i<M; ++i)
		printf("%5d\n", y[i]);
}
int main() {
    demo_mvmul();
    return 0;
}