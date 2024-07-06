#ifndef AIRPORT_H
#define AIRPORT_H

#include <pthread.h>
#include <semaphore.h>

#include "utils.h"

#define MAX_RUNWAYS 11

struct Runways {
  int numberOfRunways;
  int runwayLoadCapacity[MAX_RUNWAYS];
  sem_t *runwaySemaphores[MAX_RUNWAYS];
};

struct ThreadArgs {
  int messageQueueID;
  struct MessageBuffer messageBuffer;
  int airportID;
  struct Runways runways;
};

struct PlaneThreadArgs {
  int messageQueueID;
  struct PlaneDetails planeDetails;
  int airportID;
  struct Runways runways;
};

static void *threadFunc(void *args);
static void *departure(void *args);
static void *arrival(void *args);

#endif