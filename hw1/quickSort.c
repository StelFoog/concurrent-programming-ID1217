/**
 * @author Samuel H.E. Larsson (you@domain.com)
 * @copyright Copyright (c) 2023
 * @brief Creates a random array of number and sorts it using a multithreaded
 * quicksort algorithm
 *
 * @note |
 * Compile: `gcc quickSort.c -lpthread`
 * Execute: `./a.out <size?: number> <maxRand?: number>`
 */

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#define MAXSIZE 1000000
#define SWITCHSORT 15 /* should be between 5 and 15 */

typedef struct workerData {
  int lo;
  int hi;
} workerData;

/* timer */
double read_timer() {
  static bool initialized = false;
  static struct timeval start;
  struct timeval end;
  if (!initialized) {
    gettimeofday(&start, NULL);
    initialized = true;
  }
  gettimeofday(&end, NULL);
  return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int size;  // size of array
int array[MAXSIZE];
pthread_attr_t attr;
pthread_mutex_t safe;
pthread_cond_t go;

volatile int workersRunning = 0;
void startWorker() {
  pthread_mutex_lock(&safe);
  workersRunning++;
  pthread_mutex_unlock(&safe);
}
void endWorker(void* data) {
  free(data);
  pthread_mutex_lock(&safe);
  workersRunning--;
  if (workersRunning <= 0) pthread_cond_broadcast(&go);
  pthread_mutex_unlock(&safe);
}

workerData* workerSetup(int lo, int hi) {
  workerData* this = malloc(sizeof(workerData));
  this->lo = lo;
  this->hi = hi;
  return this;
}

void printArray(signed char f) {
  if (f < 0) {
    printf("[ ");
    for (int i = 0; i < size; i++) {
      printf("%d", array[i]);
      if (i != size - 1) printf(", ");
    }
    printf(" ]\n");
  } else if (f) {
    FILE* fptr = fopen("start", "w+");
    for (int i = 0; i < size; i++) {
      fprintf(fptr, "%7d %7d", i, array[i]);
      fprintf(fptr, ",\n");
    }
  } else {
    FILE* fptr = fopen("end", "w+");
    for (int i = 0; i < size; i++) {
      fprintf(fptr, "%7d %7d", i, array[i]);
      fprintf(fptr, ",\n");
    }
  }
}

unsigned char evalSort() {
  for (int i = 0; i < size - 1; i++) {
    if (array[i] > array[i + 1]) {
      return 0;
    }
  }
  return 1;
}

void* Worker(void*);

int main(int argc, char* argv[]) {
  /* set global thread attributes */
  double start_time, end_time;
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  pthread_mutex_init(&safe, NULL);
  pthread_cond_init(&go, NULL);
  srand(time(NULL));

  size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
  if (size > MAXSIZE || size < 0) size = MAXSIZE;
  int maxRand = (argc > 2) ? atoi(argv[2]) : INT32_MAX;
  if (maxRand > INT32_MAX || INT32_MAX < 0) maxRand = INT32_MAX;

  for (int i = 0; i < size; i++) array[i] = rand() % maxRand;
  // printArray(-1);

  // Run sort
  start_time = read_timer();

  startWorker();
  pthread_t thread;
  pthread_create(&thread, &attr, Worker, workerSetup(0, size - 1));
  // check that workers running is 0
  pthread_cond_wait(&go, &safe);

  end_time = read_timer();
  // End sort

  // printArray(-1);
  if (evalSort())
    printf("Array is verified sorted\n");
  else
    printf("Array could not be verified as sorted\n");
  printf("The execution time is %g sec\n", end_time - start_time);
}

void shortSwap(int x, int y) {
  int temp = array[x];
  array[x] = array[y];
  array[y] = temp;
}

void insertionSort(int lo, int hi) {
  for (int i = lo + 1; i <= hi; i++) {
    int x = array[i];
    int j;
    for (j = i - 1; j >= lo && array[j] > x; j--) array[j + 1] = array[j];
    array[j + 1] = x;
  }
}
// returns pivot
int quickSort(int lo, int hi) {
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

void* trash;
void* Worker(void* arg) {
  workerData* data = (workerData*)arg;
run:
  if (data->hi <= data->lo + SWITCHSORT)
    insertionSort(data->lo, data->hi);
  else {
    int pivot = quickSort(data->lo, data->hi);
    startWorker();
    pthread_t upper;
    pthread_create(&upper, &attr, Worker, workerSetup(pivot + 1, data->hi));
    data->hi = pivot - 1;
    goto run;
  }
  endWorker(data);
  return trash;
}
