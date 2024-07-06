#include "../include/table.h"

int main() {
  int waiterId, menuId;
  int* waiterPtr;
  struct Menu* menuPtr;

  int waiterNumber;
  printf("Enter waiter number (from 1 to 10): ");
  scanf(" %d", &waiterNumber);

  //   Attach to waiter shared memory.
  waiterId = shmget(waiterNumber + 200, sizeof(int), IPC_CREAT | PERMS);
  if (waiterId == -1) {
    perror("Error in shmget");
    exit(1);
  }

  waiterPtr = (int*)shmat(waiterId, NULL, 0);
  if (waiterPtr == (void*)-1) {
    perror("Error in shmat");
    exit(1);
  }

  //   Attach to menu shared memory.
  key_t key;
  if ((key = ftok(PATHNAME, PROJ_ID)) == -1) {
    perror("ftok");
    exit(1);
  }

  menuId = shmget(key, sizeof(struct Menu), PERMS);
  if (menuId == -1) {
    perror("Error in shmget");
    exit(1);
  }

  menuPtr = (struct Menu*)shmat(menuId, NULL, 0);
  if (menuPtr == (void*)-1) {
    perror("Error in shmat");
    exit(1);
  }

  while (1) {
    //   Connect to table's shared memory
    int tableNumber;
    int tableId, billId, total = 0, invalid = 0;
    struct TableOrder* tablePtr;
    struct Bill* billPtr;

    printf("Enter table number to recieve order from (type 0 to exit): ");
    scanf(" %d", &tableNumber);

    if (tableNumber == 0) {
      if (shmdt(menuPtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }

      printf("Total earnings: %d\n", waiterPtr[0]);

      if (shmdt(waiterPtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }
      if (shmctl(waiterId, IPC_RMID, 0) == -1) {
        perror("Error in shmctl");
        exit(1);
      }
      exit(0);
    }

    // Attach to TableOrder shared memory.
    tableId = shmget(tableNumber, sizeof(struct TableOrder), PERMS);
    if (tableId == -1) {
      perror("Error in shmget");
      exit(1);
    }

    tablePtr = (struct TableOrder*)shmat(tableId, NULL, 0);
    if (tablePtr == (void*)-1) {
      perror("Error in shmat");
      exit(1);
    }

    // Attach to Bill shared memory.
    billId = shmget(tableNumber + 100, sizeof(struct Bill), PERMS);
    if (billId == -1) {
      perror("Error in shmget");
      exit(1);
    }

    billPtr = (struct Bill*)shmat(billId, NULL, 0);
    if (billPtr == (void*)-1) {
      perror("Error in shmat");
      exit(1);
    }

    for (int i = 0; i < tablePtr->numberOfOrders; i++) {
      if (tablePtr->orders[i] > menuPtr->numberOfItems) {
        invalid = 1;
        billPtr->numberOfItems = -1;
        printf("Invalid order placed, item %d does not exist in menu\n",
               tablePtr->orders[i]);

        if (shmdt(tablePtr) == -1) {
          perror("Error in shmdt");
          exit(1);
        }

        if (shmdt(billPtr) == -1) {
          perror("Error in shmdt");
          exit(1);
        }
        break;
      }
      total += menuPtr->prices[tablePtr->orders[i] - 1];
    }

    if (!invalid) {
      waiterPtr[0] += total;

      billPtr->numberOfItems = tablePtr->numberOfOrders;
      billPtr->total = total;

      printf("Bill is ready for table %d\n", tablePtr->tableNumber);
      printf("Number of items ordered: %d\n", billPtr->numberOfItems);
      printf("Total bill: %d\n", billPtr->total);

      if (shmdt(tablePtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }

      if (shmdt(billPtr) == -1) {
        perror("Error in shmdt");
        exit(1);
      }
    }
  }

  exit(0);
}