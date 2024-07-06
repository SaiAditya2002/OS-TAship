#include "../include/airtrafficcontroller.h"

int main()
{
  int numberOfAirports, isTermination = 0;

  FILE *fptr;
  fptr = fopen("AirTrafficController.txt", "w");

  printf("Enter the number of airports to be handled/managed: ");
  scanf(" %d", &numberOfAirports);

  int isAirportTerminated[numberOfAirports];
  for (int i = 0; i < numberOfAirports; i++)
  {
    isAirportTerminated[i] = 0;
  }

  printf("Initializing Air Traffic Controller...\n");

  int messageQueueID;
  key_t messageQueueKey;

  if ((messageQueueKey = ftok(PATHNAME, PROJ_ID)) == -1)
  {
    perror("Error generating key in ftok");
    exit(1);
  }

  if ((messageQueueID = msgget(messageQueueKey, PERMS | IPC_CREAT)) == -1)
  {
    perror("Error creating message queue in msgget");
    exit(1);
  }

  printf("Air Traffic Controller initialized. Listening for requests.\n");

  while (1)
  {
    struct MessageBuffer messageBuffer;
    if (msgrcv(messageQueueID, &messageBuffer,
               sizeof(messageBuffer) - sizeof(messageBuffer.mtype), 1,
               0) == -1)
    {
      perror("Error receiving message in msgrcv");

      exit(1);
    }

    if (messageBuffer.sequenceNumber == 1)
    {
      if (isTermination)
      {
        struct MessageBuffer requestBuffer;
        requestBuffer.mtype = messageBuffer.planeDetails.planeID + 30;
        requestBuffer.sequenceNumber = -1;

        if (msgsnd(messageQueueID, &requestBuffer,
                   sizeof(requestBuffer) - sizeof(requestBuffer.mtype),
                   0) == -1)
        {
          perror("Error sending message in msgsnd");
          exit(1);
        }
      }
      else
      {
        struct MessageBuffer requestBuffer;
        requestBuffer.mtype = messageBuffer.planeDetails.planeID + 30;
        requestBuffer.sequenceNumber = 1;  

        if (msgsnd(messageQueueID, &requestBuffer,
                   sizeof(requestBuffer) - sizeof(requestBuffer.mtype),
                   0) == -1)
        {
          perror("Error sending message in msgsnd");
          exit(1);
        }

        struct MessageBuffer departureRequestBuffer;
        departureRequestBuffer.mtype =
            messageBuffer.planeDetails.departureAirportID + 10;
        departureRequestBuffer.sequenceNumber = 1;
        departureRequestBuffer.planeDetails = messageBuffer.planeDetails;

        if (msgsnd(messageQueueID, &departureRequestBuffer,
                   sizeof(departureRequestBuffer) -
                       sizeof(departureRequestBuffer.mtype),
                   0) == -1)
        {
          perror("Error sending message in msgsnd");
          exit(1);
        }

        struct MessageBuffer arrivalRequestBuffer;
        arrivalRequestBuffer.mtype =
            messageBuffer.planeDetails.arrivalAirportID + 10;
        arrivalRequestBuffer.sequenceNumber = 2;
        arrivalRequestBuffer.planeDetails = messageBuffer.planeDetails;

        if (msgsnd(messageQueueID, &arrivalRequestBuffer,
                   sizeof(arrivalRequestBuffer) -
                       sizeof(arrivalRequestBuffer.mtype),
                   0) == -1)
        {
          perror("Error sending message in msgsnd");
          exit(1);
        }
      }
    }

    if (messageBuffer.sequenceNumber == 2)
    {
      fprintf(
          fptr,
          "Plane %d has departed from Airport %d, and will arrive at Airport "
          "%d\n",
          messageBuffer.planeDetails.planeID,
          messageBuffer.planeDetails.departureAirportID,
          messageBuffer.planeDetails.arrivalAirportID);

      struct MessageBuffer requestBuffer;
      requestBuffer.mtype = messageBuffer.planeDetails.arrivalAirportID + 20;
      requestBuffer.sequenceNumber = 3;
      requestBuffer.planeDetails = messageBuffer.planeDetails;

      if (msgsnd(messageQueueID, &requestBuffer,
                 sizeof(requestBuffer) - sizeof(requestBuffer.mtype),
                 0) == -1)
      {
        perror("Error sending message in msgsnd");
        exit(1);
      }
    }

    if (messageBuffer.sequenceNumber == 3)
    {
      printf(
          "Plane %d successfully completed the journey from Airport %d to "
          "Airport %d\n",
          messageBuffer.planeDetails.planeID,
          messageBuffer.planeDetails.departureAirportID,
          messageBuffer.planeDetails.arrivalAirportID);

      struct MessageBuffer requestBuffer;
      requestBuffer.mtype = messageBuffer.planeDetails.planeID + 30;
      requestBuffer.sequenceNumber = 1;
      requestBuffer.planeDetails = messageBuffer.planeDetails;

      if (msgsnd(messageQueueID, &requestBuffer,
                 sizeof(requestBuffer) - sizeof(requestBuffer.mtype),
                 0) == -1)
      {
        perror("Error sending message in msgsnd");
        exit(1);
      }
    }

    if (messageBuffer.sequenceNumber == -1)
    {
      isTermination = 1;
      for (int i = 0; i < numberOfAirports; i++)
      {
        struct MessageBuffer requestBuffer;
        requestBuffer.mtype = (i + 1) + 10;
        requestBuffer.sequenceNumber = -1;

        if (msgsnd(messageQueueID, &requestBuffer,
                   sizeof(requestBuffer) - sizeof(requestBuffer.mtype),
                   0) == -1)
        {
          perror("Error sending message in msgsnd");
          exit(1);
        }
      }
    }

    if (messageBuffer.sequenceNumber == -2)
    {
      isAirportTerminated[messageBuffer.airportDetails.airportID - 1] = 1;
      printf("Airport %d terminated\n", messageBuffer.airportDetails.airportID);

      int proceedWithTerminate = 0;
      for (int i = 0; i < numberOfAirports; i++)
      {
        proceedWithTerminate += isAirportTerminated[i];
      }

      if (proceedWithTerminate == numberOfAirports)
      {
        fclose(fptr);
        printf("Removing message queue...\n");
        if (msgctl(messageQueueID, IPC_RMID, NULL) == -1)
        {
          perror("Removing queue failed");
          exit(1);
        }

        printf("Air Traffic Controller exiting...\n");
        exit(0);
      }
    }
  }
}