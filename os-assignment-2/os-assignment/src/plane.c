#include "../include/plane.h"

int main()
{
  int messageQueueID;
  key_t messageQueueKey;

  if ((messageQueueKey = ftok(PATHNAME, PROJ_ID)) == -1)
  {
    perror("Error generating key in ftok");
    exit(1);
  }

  if ((messageQueueID = msgget(messageQueueKey, PERMS)) == -1)
  {
    perror(
        "Error connecting to message queue in msgget. Is the Air Traffic "
        "Controller active?");
    exit(1);
  }

  int planeID, planeType, totalWeight = 0, numberOfOccupiedSeats = 0,
                          averageCrewWeight = 75;

  printf("Enter Plane ID: ");
  scanf(" %d", &planeID);

  printf("Enter Type of Plane: ");
  scanf(" %d", &planeType);

  if (planeType)
  {
    int numberOfCrew = 7;

    printf("Enter Number of Occupied Seats: ");
    scanf(" %d", &numberOfOccupiedSeats);

    struct Passenger passengers[numberOfOccupiedSeats];
    int fd[numberOfOccupiedSeats][2];

    for (int i = 0; i < numberOfOccupiedSeats; i++)
    {
      if (pipe(fd[i]) == -1)
      {
        perror("Error creating pipe");
        exit(1);
      }

      pid_t pid = fork();

      if (pid < 0)
      {
        perror("Error creating child process in fork");
        exit(1);
      }

      if (pid == 0)
      {
        struct Passenger passenger;

        printf("Enter Weight of Your Luggage: ");
        scanf(" %d", &passenger.luggageWeight);

        printf("Enter Your Body Weight: ");
        scanf(" %d", &passenger.bodyWeight);

        close(fd[i][0]);
        write(fd[i][1], &passenger, sizeof(passenger));
        close(fd[i][1]);

        printf("Passenger %d entered details and terminated successfully\n", i + 1);

        exit(0);
      }
      else
      {
        close(fd[i][1]);

        struct Passenger passenger;

        read(fd[i][0], &passenger, sizeof(passenger));
        close(fd[i][0]);

        passengers[i] = passenger;
      }
    }

    for (int i = 0; i < numberOfOccupiedSeats; i++)
    {
      totalWeight += passengers[i].bodyWeight + passengers[i].luggageWeight;
    }
    totalWeight += numberOfCrew * averageCrewWeight;
  }
  else
  {
    int numberOfCrew = 2, numberOfCargoItems, averageWeightOfCargoItems;

    printf("Enter Number of Cargo Items: ");
    scanf(" %d", &numberOfCargoItems);

    printf("Enter Average Weight of Cargo Items: ");
    scanf(" %d", &averageWeightOfCargoItems);

    totalWeight = numberOfCargoItems * averageWeightOfCargoItems;
    totalWeight += numberOfCrew * averageCrewWeight;
  }

  int departureAirportID, arrivalAirportID;

  printf("Enter Airport Number for Departure: ");
  scanf(" %d", &departureAirportID);

  printf("Enter Airport Number for Arrival: ");
  scanf(" %d", &arrivalAirportID);

  struct MessageBuffer departureRequestBuffer;
  departureRequestBuffer.mtype = 1;
  departureRequestBuffer.sequenceNumber = 1;
  departureRequestBuffer.planeDetails.planeID = planeID;
  departureRequestBuffer.planeDetails.planeType = planeType;
  departureRequestBuffer.planeDetails.numberOfPassengers =
      numberOfOccupiedSeats;
  departureRequestBuffer.planeDetails.totalWeight = totalWeight;
  departureRequestBuffer.planeDetails.departureAirportID = departureAirportID;
  departureRequestBuffer.planeDetails.arrivalAirportID = arrivalAirportID;

  if (msgsnd(
          messageQueueID, &departureRequestBuffer,
          sizeof(departureRequestBuffer) - sizeof(departureRequestBuffer.mtype),
          0) == -1)
  {
    perror("Error sending message in msgsnd");
    exit(1);
  }

  struct MessageBuffer departureResponseBuffer;

  if (msgrcv(messageQueueID, &departureResponseBuffer,
             sizeof(departureResponseBuffer) -
                 sizeof(departureResponseBuffer.mtype),
             planeID + 30, 0) == -1)
  {
    perror("Error receiving message in msgrcv");

    exit(1);
  }

  if (departureResponseBuffer.sequenceNumber == -1)
  {
    printf(
        "Air Traffic Control is currently awaiting termination, all departures "
        "are cancelled...\n");
    exit(0);
  }
  else
  {
    if (msgrcv(messageQueueID, &departureResponseBuffer,
               sizeof(departureResponseBuffer) -
                   sizeof(departureResponseBuffer.mtype),
               planeID + 30, 0) == -1)
    {
      perror("Error receiving message in msgrcv");

      exit(1);
    }

    if (departureResponseBuffer.sequenceNumber == 1)
    {
      printf(
          "Plane %d has successfully traveled from Airport %d to Airport %d!",
          planeID, departureAirportID, arrivalAirportID);
    }
    exit(0);
  }
}