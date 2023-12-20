#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define N 2048
#define NCORES 8
#define NTHREADS 4

float A[N][N] __attribute__((aligned(32)));
float B[N][N] __attribute__((aligned(32)));
float C[N][N];

typedef struct task
{
    short int thread_num;
    size_t from, to;
} task_t;

__m256 *Am, *Bm;

void *f(void *args)
{
    task_t *t = (task_t *)args;
    int from = t->from, to = t->to;
    for (int i = from; i < to; i++)
        for (int j = 0; j < N; j++)
        {
            __m256 t1 = {};
            __m256 t2 = {};
            int ii = (i * N) >> 3;
            int jj = (j * N) >> 3;
            int nn = N >> 3;
            __m256 *am = Am + ii, *bm = Bm + jj;
            for (int k = 0; k < nn; k += 2)
            {
                t1 = _mm256_fmadd_ps(am[0], bm[0], t1);
                t2 = _mm256_fmadd_ps(am[1], bm[1], t2);
                am += 2;
                bm += 2;
            }
            t1 = _mm256_add_ps(t1, t2);
            float *f1 = (float *)&t1;
            for (int k = 0; k < 8; k++)
                C[i][j] += f1[k];
        }
}

int main()
{
    assert(N % 8 == 0);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            A[i][j] = 1.;
            B[i][j] = 1.;
        }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            float temp = B[i][j];
            B[i][j] = B[j][i];
            B[j][i] = temp;
        }

    Am = (__m256 *)A;
    Bm = (__m256 *)B;

    pthread_t threads[NTHREADS];
    task_t args[NTHREADS];
    for (int i = 0; i < NTHREADS; i++)
    {
        args[i].thread_num = i;
        args[i].from = N / NTHREADS * i;
        args[i].to = N / NTHREADS * (i + 1);
        pthread_create(&(threads[i]), NULL, &f, (void *)&args[i]);
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(i % NCORES, &cpuset);
        pthread_setaffinity_np(threads[i], sizeof(cpuset), &cpuset);
    }

    for (int i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);
    
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double time_difference = seconds + microseconds / 1000000.0;

    printf("%.2f\n", time_difference);

#ifdef DEBUG
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%.2f ", C[i][j]);
        puts("");
    }
#endif

    return 0;
}
