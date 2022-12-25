#include <stdio.h>
#include <assert.h>
#include <string.h>
#define M (32)
#define N (32)
#define K (32)

int mm_basic(float a[M][K], float b[K][N], float c[M][N])
{
    const int BN_M = 2;
    const int BN_N = 2;
    const int BN_K = 2;
    
    int BS_M = M / BN_M;
    int BS_N = N / BN_N;
    int BS_K = K / BN_K;

    assert(M % BN_M == 0);
    assert(N % BN_N == 0);
    assert(K % BN_K == 0);

    for (int m = 0; m < M; m++)
    {
        for (int n = 0; n < N; n++)
        {
            c[m][n] = 0;
        }
    }
    for (int bm = 0; bm < BN_M; bm++)
    {
        for (int bn = 0; bn < BN_N; bn++)
        {
            for (int m = 0; m < BS_M; m++)
            {
                for (int n = 0; n < BS_N; n++)
                {
                    for (int bk = 0; bk < BN_K; bk++)
                    {
                        // c[m][n] = 0;
                        for (int k = 0; k < BS_K; k++)
                        {
                            int m2 = m + BS_M * bm;
                            int n2 = n + BS_N * bn;
                            int k2 = k + BS_K * bk;
                            c[m2][n2] += a[m2][k2] * b[k2][n2];
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void print(float a[M][N])
{
    for (int m = 0; m < M; m++)
    {
        for (int n = 0; n < N; n++)
        {
            printf("%f ", a[m][n]);
        }
    }
}

int main()
{
    float a[M][K]; 
    float b[K][N]; 
    float c[M][N]; 
    for (int m = 0; m < M; m++)
    {
        for (int k = 0; k < K; k++)
        {
            a[m][k] = 1;
        }
    }
    for (int k = 0; k < K; k++)
    {
        for (int n = 0; n < N; n++)
        {
            b[k][n] = 1;
        }
    }    
    mm_basic(a, b, c);

    print(c);

    return 0;
}
