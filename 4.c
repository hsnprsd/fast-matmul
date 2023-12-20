#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <immintrin.h>

// #define DEBUG 1

#define N 2048
#define NRUNS 10

float A[N][N] __attribute__((aligned(32)));
float B[N][N] __attribute__((aligned(32)));
float C[N][N];

__m256 *Am, *Bm;

void transpose(float M[N][N])
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            float temp = M[i][j];
            M[i][j] = M[j][i];
            M[j][i] = temp;
        }
}

int main()
{
    assert(N % 8 == 0);

    for (int i = 0; i < N; i++)
    {
        // A[i][j] = (float)rand() / RAND_MAX;
        // B[i][j] = (float)rand() / RAND_MAX;
        A[i][i] = 1.;
        B[i][i] = 1.;
    }

    struct timeval start_t, end_t;

    float avg = 0;

    for (int run = 0; run < NRUNS; run++)
    {
        gettimeofday(&start_t, NULL);

        Am = (__m256 *)A;
        Bm = (__m256 *)B;

        transpose(B);

        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                __m256 acc1 = {}, acc2 = {};

                int ii = (i * N) >> 3;
                int jj = (j * N) >> 3;
                int nn = N >> 3;
                __m256 *am = Am + ii, *bm = Bm + jj;

                for (int k = 0; k < nn; k += 2, am += 2, bm += 2)
                {
                    acc1 = _mm256_fmadd_ps(am[0], bm[0], acc1);
                    acc2 = _mm256_fmadd_ps(am[1], bm[1], acc2);
                }

                acc1 = _mm256_add_ps(acc1, acc2);
                float *f = (float *)&acc1;
                for (int k = 0; k < 8; k++)
                    C[i][j] += f[k];
            }

        gettimeofday(&end_t, NULL);

        long s = end_t.tv_sec - start_t.tv_sec;
        long ms = end_t.tv_usec - start_t.tv_usec;
        double diff = s * 1000 + ms / 1000.;

        avg += diff;

        printf("%.2f\n", diff);

        if (run != NRUNS - 1)
            memset(C, 0, sizeof(C));
    }

    avg /= NRUNS;
    printf("AVG = %.2f\n", avg);

#ifdef DEBUG
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%.2f ", C[i][j]);
        puts("");
    }
#endif

    return 0;
}
