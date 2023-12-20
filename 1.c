#include <stdio.h>
#include <sys/time.h>
#include <string.h>

// #define DEBUG 1

#define N 2048
#define NRUNS 10

float A[N][N];
float B[N][N];
float C[N][N];

int main()
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            A[i][j] = 1.;
            B[i][j] = 1.;
        }

    struct timeval start_t, end_t;

    float avg = 0;

    for (int run = 0; run < NRUNS; run++) {
        gettimeofday(&start_t, NULL);

        for (int i = 0; i < N; i++)
            for (int k = 0; k < N; k++)
                for (int j = 0; j < N; j++)
                    C[i][j] += A[i][k] * B[k][j];

        gettimeofday(&end_t, NULL);

        long s = end_t.tv_sec - start_t.tv_sec;
        long ms = end_t.tv_usec - start_t.tv_usec;
        double diff = s * 1000 + ms / 1000.;

        avg += diff;

        printf("%.2f\n", diff);

        if (run != NRUNS - 1) memset(C, 0, sizeof(C));
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
