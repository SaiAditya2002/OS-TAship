#include "../include/table.h"

int main() {
  // Connect to table's shared memory
  int tableNumber;
  int tableId, billId;
  struct TableOrder *tablePtr;
  struct Bill *billPtr;

  printf("Enter table number (from 1 to 100): ");
  scanf(" %d", &tableNumber);

  while (1) {
    // Attach to TableOrder shared memory.
    tableId = shmget(tableNumber, sizeof(struct TableOrder), IPC_CREAT | PERMS);
    if (tableId == -1) {
      perror("Error in shmget");
      exit(1);
    }

    tablePtr = (struct TableOrder *)shmat(tableId, NULL, 0);
    if (tablePtr == (void *)-1) {
      perror("Error in shmat");
      exit(1);
    }

    // Attach to Bill shared memory.
    billId = shmget(tableNumber + 100, sizeof(struct Bill), IPC_CREAT | PERMS);
    if (billId == -1) {
      perror("Error in shmget");
      exit(1);
    }

    billPtr = (struct Bill *)shmat(billId, NULL, 0);
    if (billPtr == (void *)-1) {
      perror("Error in shmat");
      exit(1);
    }

    tablePtr->tableNumber = tableNumber;
    printf("Enter number of orders (maximum %d, -1 to exit): ", MAX_ORDER_SIZE);
    scanf(" %d", &(tablePtr->numberOfOrders));

    if (tablePtr->numberOfOrders == -1) {
      // Detach and delete TableOrder shared memory.
      if (shmdt(tablePtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }

      if (shmctl(tableId, IPC_RMID, 0) == -1) {
        perror("Error in shmctl");
        exit(1);
      }

      // Detach and delete Bill shared memory.
      if (shmdt(billPtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }

      if (shmctl(billId, IPC_RMID, 0) == -1) {
        perror("Error in shmctl");
        exit(1);
      }

      break;
    }

    int customers = tablePtr->numberOfOrders;
    while (customers--) {
      int fd[2];
      if (pipe(fd) == -1) {
        perror("Error creating pipe");
        exit(1);
      }

      pid_t pid = fork();

      if (pid < 0) {
        perror("Error creating child process in fork");
        exit(1);
      }

      if (pid == 0) {
        printf("Enter your choice for order %d: ",
               tablePtr->numberOfOrders - customers);
        int choice;
        scanf(" %d", &choice);

        close(fd[0]);
        write(fd[1], &choice, sizeof(choice));
        close(fd[1]);

        exit(0);
      }
      int childStatus;
      wait(&childStatus);
      close(fd[1]);
      if (childStatus == 0) {
        int choice;

        read(fd[0], &choice, sizeof(choice));
        close(fd[0]);

        tablePtr->orders[customers] = choice;
      } else {
        printf("Failed to place order\n");
      }
    }

    char input = 'N';
    while (input != 'Y' && input != 'y') {
      if (input != 'N' && input != 'n') {
        printf("Invalid input\n");
      }
      printf("Press Y to receive the bill.\n");
      scanf(" %c", &input);
    }

    if (billPtr->numberOfItems == -1) {
      printf("Invalid order placed, please place the correct order\n");

      // Detach and delete TableOrder shared memory.
      if (shmdt(tablePtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }

      if (shmctl(tableId, IPC_RMID, 0) == -1) {
        perror("Error in shmctl");
        exit(1);
      }

      // Detach and delete Bill shared memory.
      if (shmdt(billPtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }

      if (shmctl(billId, IPC_RMID, 0) == -1) {
        perror("Error in shmctl");
        exit(1);
      }

      continue;
    }

    printf("Number of iterms ordered: %d\n", billPtr->numberOfItems);
    printf("Total bill cost: %d\n", billPtr->total);

    // Detach and delete TableOrder shared memory.
    if (shmdt(tablePtr) == -1) {
      perror("Error in shmdt");
      exit(1);
    }

    if (shmctl(tableId, IPC_RMID, 0) == -1) {
      perror("Error in shmctl");
      exit(1);
    }

    // Detach and delete Bill shared memory.
    if (shmdt(billPtr) == -1) {
      perror("Error in shmdt");
      exit(1);
    }

    if (shmctl(billId, IPC_RMID, 0) == -1) {
      perror("Error in shmctl");
      exit(1);
    }
  }

  exit(0);
}