#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAX 500

double matrix1[MAX][MAX], matrix2[MAX][MAX], matrix3[MAX][MAX];
int raw = MAX, col = MAX;

void *multithread(void *arg)
{
   int start = *((int *)arg);
   start = start * 5;
   int end = start + 5;
   double val;
   for (int i = start; i < end; i++)
   {
      for (int j = 0; j < col; j++)
      {
         val = 0;
         for (int k = 0; k < raw; k++)
         {
            val += (matrix1[i][k] * matrix2[k][j]);
         }
         matrix3[i][j] = val;
      }
   }
   free(arg);
   pthread_exit(NULL);
}

int main()
{
   int i, j, x;
   double val;
   char infilename[20], outfilename[20];
   FILE *infile, *outfile;
   struct timespec tp_s, tp_e;
   struct timezone tzp_s, tzp_e;
   gettimeofday(&tp_s, &tzp_s);
   for (x = 0; x < 1000; x++)
   {
      printf("%d\n", x);
      sprintf(infilename, "in%d.txt", x + 1);
      sprintf(outfilename, "out%d.txt", x + 1);

      if ((infile = fopen(infilename, "r")) == NULL)
      {
         perror("in open error~\n");
         exit(EXIT_FAILURE);
      }

      fscanf(infile, "%d", &raw);
      fscanf(infile, "%d", &col);

      for (i = 0; i < raw; i++)
      {
         for (j = 0; j < col; j++)
         {
            fscanf(infile, "%lf", &val);
            matrix1[i][j] = matrix2[i][j] = val;
         }
      }

      if ((outfile = fopen(outfilename, "w")) == NULL)
      {
         perror("out open error~\n");
         exit(EXIT_FAILURE);
      }

      fprintf(outfile, "%d\n%d\n", raw, col);

      pthread_t p[100];

      for (int i = 0; i < 100; i++)
      {
         int *arg = malloc(sizeof(*arg));
         *arg = i;
         pthread_create(&p[i], NULL, multithread, arg);
      }

      for (int i = 0; i < 100; i++)
      {
         pthread_join(p[i], NULL);
      }

      for (i = 0; i < raw; i++)
      {
         for (j = 0; j < col; j++)
         {
            fprintf(outfile, "%lf\n", matrix3[i][j]);
         }
      }

      fclose(infile);
      fclose(outfile);
   }

   gettimeofday(&tp_e, &tzp_e);
   if ((outfile = fopen("time.txt", "w")) == NULL)
   {
      perror("out open error~\n");
      exit(EXIT_FAILURE);
   }

   fprintf(outfile, "Total execution time = %ld\n", tp_e.tv_sec - tp_s.tv_sec);
   fclose(outfile);

   return 0;
}
