/**
 * @author Samuel H.E. Larsson
 * @copyright Copyright (c) 2023
 * @brief Creates a matrix filled with random numbers and finds the maximum and
 * minimum values and calculates the total of all elements using OpenMP
 *
 * @note |
 * Compile: `clang -fopenmp -o matrixSum-omp matrixSum-omp.c`
 * Execute: `./matrixSum-omp <size?: number> <numWorkers?: number>`
 */

#include <omp.h>

double start_time, end_time;

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE 1000000 /* maximum matrix size */
#define MAXWORKERS 32   /* maximum allowed workers */
#define MAXRAND 10000   /* maximum random number */
#define RUNS 5          /* Runs per size and threads */

int size;
int maxWorkers;
int matrix[MAXSIZE][MAXSIZE];

double medianTime(double arr[], int runs) {
  for (int i = 0; i < runs; i++) {
    for (int j = i + 1; j < runs; j++) {
      if (arr[j] < arr[i]) {
        double temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
    printf("%g ", arr[i]);
  }
  return arr[runs / 2];
}

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;

  unsigned int total = 0;
  unsigned int min_x = 0, min_y = 0, max_x = 0, max_y = 0;
  unsigned int shared_min_x = 0, shared_min_y = 0, shared_max_x = 0,
               shared_max_y = 0;

  /* read command line args if any */
  size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
  if (size > MAXSIZE || size < 0) size = MAXSIZE;
  maxWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
  if (maxWorkers > MAXWORKERS || size < 0) size = MAXWORKERS;

  for (int ts = 1; ts <= maxWorkers; ts = ts * 2) {
    omp_set_num_threads(ts);

    for (int s = 10; s <= size; s = s * 10) {
      /* initialize the matrix */
      for (i = 0; i < s; i++) {
        for (j = 0; j < s; j++) {
          matrix[i][j] = rand() % MAXRAND;
        }
      }

      double time[RUNS];
      for (int run = 0; run < RUNS; run++) {
        total = 0;
        min_x = 0, min_y = 0, max_x = 0, max_y = 0;
        shared_min_x = 0, shared_min_y = 0, shared_max_x = 0, shared_max_y = 0;

        start_time = omp_get_wtime();

#pragma omp parallel shared(shared_min_x, shared_min_y, shared_max_x, \
                            shared_max_y) private(j)                  \
    firstprivate(min_x, min_y, max_x, max_y)
        {
#pragma omp for reduction(+ : total) nowait
          for (i = 0; i < s; i++) {
            for (j = 0; j < s; j++) {
              total += matrix[i][j];
              if (matrix[min_x][min_y] > matrix[i][j]) {
                min_x = i;
                min_y = j;
              }
              if (matrix[max_x][max_y] < matrix[i][j]) {
                max_x = i;
                max_y = j;
              }
            }
          }
#pragma omp critical
          {
            if (matrix[shared_min_x][shared_min_y] > matrix[min_x][min_y]) {
              shared_min_x = min_x;
              shared_min_y = min_y;
            }
            if (matrix[shared_max_x][shared_max_y] < matrix[max_x][max_y]) {
              shared_max_x = max_x;
              shared_max_y = max_y;
            }
          }
        }
        // implicit barrier

        end_time = omp_get_wtime();
        time[run] = end_time - start_time;
      }

      printf("the total is %u\n", total);
      printf("the min is %d @ x=%d, y=%d\n", matrix[shared_min_x][shared_min_y],
             shared_min_x, shared_min_y);
      printf("the max is %d @ x=%d, y=%d\n", matrix[shared_max_x][shared_max_y],
             shared_max_x, shared_max_y);
      printf("using %d threads on a matrix of size %d\n", ts, s);
      printf("During %d runs it took a median of %lf seconds\n\n", RUNS,
             medianTime(time, RUNS));
    }
  }
}
