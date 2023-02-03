/**
 * @author Samuel H.E. Larsson (you@domain.com)
 * @copyright Copyright (c) 2023
 * @brief Randomises a series of points within a 1x1 square and calculates pi
 * based on them
 *
 * @note |
 * Compile: `gcc computePi.c -lpthread`
 * Execute: `./a.out <workers?: number> <points?: number>`
 */

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#define MAXWORKERS 16
#define MAXPOINTS 100000000

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

short workers;
int points;
unsigned long res[MAXWORKERS];  // size of array
pthread_attr_t attr;
pthread_mutex_t safe;
pthread_cond_t go;

volatile int workersDone = 0;
void endWorker() {
  pthread_mutex_lock(&safe);
  workersDone++;
  if (workersDone >= workers) pthread_cond_broadcast(&go);
  pthread_mutex_unlock(&safe);
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
  pthread_t threads[MAXWORKERS];

  workers = (argc > 1) ? atoi(argv[1]) : MAXWORKERS;
  if (workers > MAXWORKERS || workers < 0) workers = MAXWORKERS;
  points = (argc > 2) ? atoi(argv[2]) : MAXPOINTS;
  if (points > MAXPOINTS || points < 0) points = MAXPOINTS;

  // Compute pi
  start_time = read_timer();

  pthread_mutex_lock(&safe);
  for (long i = 0; i < workers; i++)
    pthread_create(&threads[i], &attr, Worker, (void*)i);
  pthread_cond_wait(&go, &safe);
  unsigned long totalInside = 0;
  for (int i = 0; i < workers; i++) totalInside += res[i];
  double pi = (double)4 * totalInside / (workers * points);

  end_time = read_timer();
  // Computed pi

  printf("%ld / ( %d * %d )\n", totalInside, workers, points);
  printf("Pi is %f (roughly)\n", pi);
  printf("The execution time is %g sec\n", end_time - start_time);
}

void* trash;
void* Worker(void* arg) {
  int id = (int)arg;
  double x, y;
  for (int i = 0; i < points; i++) {
    x = (float)rand() / RAND_MAX;
    y = (float)rand() / RAND_MAX;
    if (((x * x) + (y * y)) <= 1) res[id]++;
  }
  endWorker();
  return trash;
}
