#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define PERMS 0644
#define PATHNAME "./src/airtrafficcontroller.c"
#define PROJ_ID 'C'

struct Passenger {
  int luggageWeight;
  int bodyWeight;
};

struct AirportDetails {
  int airportID;
};

struct PlaneDetails {
  int planeID;
  int planeType;
  int numberOfPassengers;
  int totalWeight;
  int departureAirportID;
  int arrivalAirportID;
};

struct MessageBuffer {
  long mtype;
  int sequenceNumber;
  struct AirportDetails airportDetails;
  struct PlaneDetails planeDetails;
};

#endif