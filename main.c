#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#define MAX 500

typedef struct
{
    int row;
    int col;
    long (*matrix1)[MAX];
    long (*matrix2)[MAX];
    int thread_id;
} ThreadData;

void *matrix_multiply(void *arg)
{

    ThreadData *data = (ThreadData *)arg;
    long matrix3[MAX][MAX]={};
    FILE *outfile;
    char outfilename[20];

    for (int i = 0; i < data->row; i++)
    {
        for (int j = 0; j < data->col; j++)
        {
            long t = data->matrix1[i][j];
            for (int k = 0; k < data->row; k++)
            {
                matrix3[i][k] += t * data->matrix2[j][k];
            }
        }
    }
    sprintf(outfilename, "out%d.txt", data->thread_id + 1);
    if ((outfile = fopen(outfilename, "w")) == NULL)
        perror("out open error~\n");

    fprintf(outfile, "%d\n%d\n", data->row, data->col);
    for (int i = 0; i < data->row; i++)
    {
        for (int j = 0; j < data->col; j++)
        {
            fprintf(outfile, "%ld\n", matrix3[i][j]);
        }
    }
    fclose(outfile);
    free(data);
    pthread_exit(NULL);
}

int main()
{
    long matrix1[MAX][MAX], matrix2[MAX][MAX];
    char infilename[20];
    struct timeval tp_s, tp_e;
    struct timezone tzp_s, tzp_e;
    int row, col, i, j;
    FILE *outfile, *infile;
    gettimeofday(&tp_s, &tzp_s);
    pthread_t *threads;
    threads = (pthread_t *) malloc(1000 * sizeof(pthread_t));
    for (int x = 0; x < 1000; x++)
    {
        printf("%d\n", x);
        sprintf(infilename, "in%d.txt", x + 1);
        if ((infile = fopen(infilename, "r")) == NULL)
            perror("in open error~\n");
        fscanf(infile, "%d", &row);
        fscanf(infile, "%d", &col);
        for (i = 0; i < row; i++)
        {
            for (j = 0; j < col; j++)
            {
                fscanf(infile, "%ld", &matrix1[i][j]);
                matrix2[i][j] = matrix1[i][j];
            }
        }
        fclose(infile);
        ThreadData *data = malloc(sizeof(ThreadData));
        data->row = row;
        data->col = col;
        data->matrix1 = malloc(sizeof(long) * MAX * MAX);
        data->matrix2 = malloc(sizeof(long) * MAX * MAX);
        memcpy(data->matrix1, matrix1, sizeof(long) * MAX * MAX);
        memcpy(data->matrix2, matrix2, sizeof(long) * MAX * MAX);
        data->thread_id = x;
        if (pthread_create(&threads[x], NULL, matrix_multiply, data) != 0)
        {
            perror("Failed to create thread");
            free(data);
        }
    }
    for (int x = 0; x < 1000; x++)
    {
        pthread_join(threads[x], NULL);
    }
    gettimeofday(&tp_e, &tzp_e);
    printf("Total execution time =%ld\n", tp_e.tv_sec - tp_s.tv_sec);
    if ((outfile = fopen("time.txt", "w")) == NULL)
        perror("out open error~\n");
    fprintf(outfile, "Total execution time = %ld\n", tp_e.tv_sec - tp_s.tv_sec);
    fclose(outfile);
    return 0;
}