#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <omp.h>
#include <sys/timeb.h>
#if defined(WIN32)
#else
#define TIMEB timeb
typedef long long TIME_T;
#endif

void init(float *a, int M, int N, bool zero_init)
{
    int m, n;
    for (m = 0; m < M; m++)
    {
        for (n = 0; n < N; n++)
        {
            if (zero_init)
                a[m*N + n] = 0;
            else
                a[m*N + n] = 1;
        }
        
    }
}

int mm_block(float *a, float *b, float *c, int M, int N, int K, int LDA, int LDB, int LDC)
{
    int m,n,k;
    for(m = 0; m < M; m++)
    {
        for (n = 0; n < N; n++)
        {
            c[m*LDC+n] = 0;
            for(k = 0; k < K; k++)
            {
                c[m*LDC+n] += a[m*LDA+k] * b[k*LDB+n];
            }
        }
    }
    return 0;
}



int mm(float *a, float *b, float *c, int M, int N, int K)
{
    int cm,cn;
    const int CM = 4;
    const int CN = 4;
    assert(M % CM == 0);
    assert(N % CN == 0);

    //init(c, M, N, true);

    // partition vertically
/*
#pragma omp parallel for num_threads(4)
    for (cn = 0; cn < CN; cn++)
    {
        mm_block(a, b+N/CN*cn, c+N/CN*cn, M, N/CN, K, K, N, N);
    }
*/    
    // partition horizontally
    /*for (cm = 0; cm < CM; cm++)
    {
        mm_block(a+M/CM*cm*K, b, c+M/CM*cm*N, M/CM, N, K, K, N, N);
    }*/

    // partition vertically + horizontally


#pragma omp parallel for num_threads(4)
    for (cm = 0; cm < CM; cm++) 
    {      
        for (cn = 0; cn < CN; cn++)
        {
            mm_block(a+M/CM*K*cm, b+N/CN*cn, c+M/CM*cm*N+N/CN*cn, M/CM, N/CN, K, K, N, N);
        }
    }

    return 0;
}

void print(float *a, int M, int N)
{
    int m, n;
    for (m = 0; m < M; m++)
    {
        for (n = 0; n < N; n++)
        {
            printf("%4g ", a[m*N+n]);
        }
        printf("\n");
    }
}


float* matrix_malloc(size_t size)
{
    float* p = (float*)malloc(size);
    if (NULL == p)
    {
        abort();
    }
    return p;
}

int mm_basic(float *a, float *b, float *c, int M, int N, int K)
{
    int m,n,k;

    init(c, M, N, true);
    for(m = 0; m < M; m++)
    {
        for(n = 0; n < N; n++)
        {
            //c[m*N+n] = 0;
            for(k = 0; k < K; k++)
            {
                c[m*N+n] += a[m*K+k] * b[k*N+n]; 
            }
        }
    }
    return 0;
}

int check(float *a, float *b, int M , int N)
{
    int m,n;
    int count = 0;
    for (m = 0; m < M; m++)
    {
        for (n = 0; n < N; n++)
        {
            if (a[m*N+n] == b[m*N+n])
            {
            }
            else
            {
                if (count < 10)
                    printf("%d: a: %f, b: %f\n", m*N+n, a[m*N+n], b[m*N+n]);
                count++;
            }
        }
    }
    if (count == 0) {
        printf("success\n");
        return 0;
    } else {
        printf("Number of difference: %d\n", count);
    }
    return -1;
}

#define TIME_IT(func, name)  \
do { \
    struct TIMEB ts1, ts2; \
    TIME_T t1, t2; \
    TIME_T ti; \
    ftime(&ts1); \
    func; \
    ftime(&ts2); \
    t1 = ts1.time*1000 + ts1.millitm; \
    t2 = ts2.time*1000 + ts2.millitm; \
    ti = t2 - t1; \
    printf(#name ":%lldms\n", ti); \
} while(0)

int main(int argc, char **argv)
{
    float *a = NULL;
    float *b = NULL;
    float *c1 = NULL;
    float *c2 = NULL;
    const int M = 256;
    const int N = 256;
    const int K = 256;

    a = matrix_malloc(sizeof(float)*M*K);
    b = matrix_malloc(sizeof(float)*K*N);
    c1 = matrix_malloc(sizeof(float)*M*N);
    c2 = matrix_malloc(sizeof(float)*M*N);

    init(a, M, K, false);
    init(b, K, N, false);
    init(c1, M, N, true);
    init(c2, M, N, true);

    TIME_IT(
        {
            for (int i = 0; i < 10; i++)
                mm_basic(a, b, c1, M, N, K);
        },
        "mm_basic");
    TIME_IT(
        {
            for (int i = 0; i < 10; i++)
                mm(a, b, c2, M, N, K);
        },
        "mm");

    check(c1, c2, M, N);

    free(a);
    free(b);
    free(c1);
    free(c2);
    return 0;
} 