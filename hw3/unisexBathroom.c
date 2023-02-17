/**
 * @author Samuel H.E. Larsson
 * @copyright Copyright (c) 2023
 * @details Assume there's a bathroom where any number of men or women can enter
 * the bathroom at the same time, but there can never be a woman and man in the
 * bathroom at the same time.
 * @brief Creates a set of theads representing men and women who all need to go
 * to the bathroom a set amount of times. For each visit to the bathroom, let
 * the person work for a random amount of time and then try to go to the
 * bathroom. Implemented using semaphores
 *
 * @note |
 * Compile: `gcc unisexBathroom.c -lpthread -o unisexBathroom`
 * Execute: `./unisexBathroom <visits?: number> <men?: number> <women?: number>`
 */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_MEN 6      /* maximum number of men */
#define MAX_WOMEN 6    /* maximum number of women */
#define MAX_VISITS 100 /* maximum number of visits each person makes */
#define MAX_WORK_TIME 7
#define MIN_WORK_TIME 2
#define MAX_BATHROOM_TIME 3
#define MIN_BATHROOM_TIME 1

sem_t critical;
sem_t manWait;
sem_t womanWait;

int menInBathroom = 0;
int menWaiting = 0;
int womenInBathroom = 0;
int womenWaiting = 0;

int menLeaving = false;
int womenLeaving = false;

int numMen, numWomen, numVisits;

void* male(void*);
void* female(void*);
int randomRange(int, int);

int main(int argc, char* argv[]) {
  pthread_t men[MAX_MEN], women[MAX_WOMEN];

  sem_init(&critical, 0, 1);
  sem_init(&manWait, 0, 0);
  sem_init(&womanWait, 0, 0);

  srand(time(NULL));  // Remove to always have same random values

  numVisits = argc > 1 ? atoi(argv[1]) : MAX_VISITS;
  if (numVisits > MAX_VISITS || numVisits < 0) numVisits = MAX_VISITS;
  numMen = argc > 2 ? atoi(argv[2]) : MAX_MEN;
  if (numMen > MAX_MEN || numMen < 0) numMen = MAX_MEN;
  numWomen = argc > 2 ? atoi(argv[2]) : MAX_WOMEN;
  if (numWomen > MAX_WOMEN || numWomen < 0) numWomen = MAX_WOMEN;

  for (long i = 0; i < numMen; i++)
    pthread_create(&(men[i]), NULL, male, (void*)i);
  for (long i = 0; i < numWomen; i++)
    pthread_create(&(women[i]), NULL, female, (void*)i);

  for (long i = 0; i < numMen; i++) pthread_join(men[i], NULL);
  for (long i = 0; i < numWomen; i++) pthread_join(women[i], NULL);

  printf("All visits complete\n");

  sem_close(&critical);
  sem_close(&manWait);
  sem_close(&womanWait);

  return 0;
}

void* male(void* arg) {
  long id = (long)arg;

  for (int visit = 0; visit < numVisits; visit++) {
    sleep(randomRange(MIN_WORK_TIME, MAX_WORK_TIME));

    sem_wait(&critical);
    if (womenInBathroom > 0 || (menLeaving && womenWaiting > 0)) {
      // Has to wait
      menWaiting++;
      sem_post(&critical);
      sem_wait(&manWait);
    }
    menInBathroom++;

    if (menWaiting > 0) {
      menWaiting--;
      sem_post(&manWait);
    } else {
      sem_post(&critical);
    }

    int bathroomTime = randomRange(MIN_BATHROOM_TIME, MAX_BATHROOM_TIME);
    printf("🚹 #%ld enters bathroom - Visit %d\n", id + 1, visit + 1);
    sleep(bathroomTime);  // Bathroom time
    printf("🚹 #%ld left after %ds\n", id + 1, bathroomTime);

    sem_wait(&critical);
    menInBathroom--;
    menLeaving = true;

    if (menInBathroom == 0) menLeaving = false;
    if (menInBathroom == 0 && womenWaiting > 0) {
      womenWaiting--;
      sem_post(&womanWait);
    } else {
      sem_post(&critical);
    }
  }

  return NULL;
}

void* female(void* arg) {
  long id = (long)arg;

  for (int visit = 0; visit < numVisits; visit++) {
    sleep(randomRange(MIN_WORK_TIME, MAX_WORK_TIME));

    sem_wait(&critical);
    if (menInBathroom > 0 || (womenLeaving && menWaiting > 0)) {
      // Has to wait
      womenWaiting++;
      sem_post(&critical);
      sem_wait(&womanWait);
    }
    womenInBathroom++;

    if (womenWaiting > 0) {
      womenWaiting--;
      sem_post(&womanWait);
    } else {
      sem_post(&critical);
    }

    int bathroomTime = randomRange(MIN_BATHROOM_TIME, MAX_BATHROOM_TIME);
    printf("🚺 #%ld enters bathroom - Visit %d\n", id + 1, visit + 1);
    sleep(bathroomTime);  // Pee
    printf("🚺 #%ld left after %ds\n", id + 1, bathroomTime);

    sem_wait(&critical);
    womenInBathroom--;
    womenLeaving = true;

    if (womenInBathroom == 0) womenLeaving = false;
    if (womenInBathroom == 0 && menWaiting > 0) {
      menWaiting--;
      sem_post(&manWait);
    } else {
      sem_post(&critical);
    }
  }

  return NULL;
}

int randomRange(int min, int max) {
  int limit = max - min + 1;
  return (rand() % limit) + min;
}
