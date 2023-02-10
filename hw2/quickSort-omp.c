/**
 * @author Samuel H.E. Larsson
 * @copyright Copyright (c) 2023
 * @brief Creates an array of random numbers and sorts it with quicksort using
 * OpenMP
 *
 * @note |
 * Compile: `clang -O -fopenmp -o quickSort-omp quickSort-omp.c`
 * Execute: `./quickSort-omp <size?: number> <numWorkers?: number>`
 */

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXSIZE 1000000
#define MAXRANDOM 1000000
#define MAXTHREADS 32
#define SWITCHSORT 15 /* should be between 5 and 15 */
#define RUNS 5        /* amount of runs per size and workers */

int size;  // size of array
int maxThreads;
int array[MAXSIZE];

double medianTime(double arr[], int runs) {
  for (int i = 0; i < runs / 2; i++) {
    for (int j = i + 1; j < runs; j++) {
      if (arr[j] < arr[i]) {
        double temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
  }
  return arr[runs / 2];
}

unsigned char evalSort(int size) {
  for (int i = 0; i < size - 1; i++) {
    if (array[i] > array[i + 1]) {
      return 0;
    }
  }
  return 1;
}

void quickSort(int, int);

int main(int argc, char* argv[]) {
  /* set global thread attributes */
  double start_time, end_time;
  double sum_time;
  srand(time(NULL));
  omp_set_dynamic(0);

  size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
  if (size > MAXSIZE || size < 100) size = MAXSIZE;
  maxThreads = (argc > 2) ? atoi(argv[2]) : MAXTHREADS;
  if (maxThreads > MAXTHREADS || maxThreads < 0) maxThreads = MAXSIZE;
  int maxRand = (argc > 3) ? atoi(argv[2]) : MAXRANDOM;

  for (int threads = 1; threads <= maxThreads; threads *= 2) {
    omp_set_num_threads(threads);
    for (int localSize = 100; localSize <= size; localSize = localSize * 10) {
      double times[RUNS];
      for (int run = 0; run < RUNS; run++) {
        // Create a new array
        for (int i = 0; i < localSize; i++) array[i] = rand() % maxRand;

        start_time = omp_get_wtime();
        // Run sort
#pragma omp task
        quickSort(0, localSize - 1);
#pragma omp taskwait
        // End sort
        end_time = omp_get_wtime();
        times[run] = end_time - start_time;
      }
      if (!evalSort(localSize)) {
        printf("Array could not be verified as sorted\n");
        goto kill;
      }
      printf("The median execution time is %gs on size %d with %d threads\n",
             medianTime(times, RUNS), localSize, threads);
    }
    printf("\n");
  }
kill:;
  // printArray(-1);
}

void shortSwap(int x, int y) {
  int temp = array[x];
  array[x] = array[y];
  array[y] = temp;
}

void insertionSort(int lo, int hi) {
  for (int i = lo + 1; i <= hi; i++) {
    // printf("%d\n", i);
    int x = array[i];
    int j;
    for (j = i - 1; j >= lo && array[j] > x; j--) array[j + 1] = array[j];
    array[j + 1] = x;
  }
}

// returns pivot
int pivot(int lo, int hi) {
  int mid = (lo + hi) / 2;
  if (array[mid] < array[lo]) shortSwap(mid, lo);
  if (array[hi] < array[lo]) shortSwap(hi, lo);
  if (array[mid] < array[hi]) shortSwap(mid, hi);
  int pivot = array[hi];
  int i = lo;
  for (int j = lo; j <= hi; j++) {
    if (array[j] < pivot) {
      shortSwap(i, j);
      i++;
    }
  }
  shortSwap(i, hi);
  return i;
}

void quickSort(int lo, int hi) {
  // printf("lo=%3d hi=%3d\n", lo, hi);
  if (hi <= lo + SWITCHSORT) {
    insertionSort(lo, hi);
  } else {
    int p = pivot(lo, hi);
#pragma omp task
    quickSort(lo, p - 1);
#pragma omp task
    quickSort(p + 1, hi);
  }
}
