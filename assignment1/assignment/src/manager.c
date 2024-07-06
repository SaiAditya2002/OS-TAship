#include "../include/manager.h"

int main() {
  int menuId;
  struct Menu* menuPtr;

  int totalProfit = 0;

  key_t key;
  if ((key = ftok(PATHNAME, PROJ_ID)) == -1) {
    perror("ftok");
    exit(1);
  }

  //   Attach to shared memory.
  menuId = shmget(key, sizeof(struct Menu), IPC_CREAT | PERMS);
  if (menuId == -1) {
    perror("Error in shmget");
    exit(1);
  }

  menuPtr = (struct Menu*)shmat(menuId, NULL, 0);
  if (menuPtr == (void*)-1) {
    perror("Error in shmat");
    exit(1);
  }

  printf("Please enter the number of items in the menu: ");
  scanf(" %d", &(menuPtr->numberOfItems));

  int price;
  for (int i = 0; i < menuPtr->numberOfItems; i++) {
    printf("Please enter the price for item %d: ", i + 1);
    scanf(" %d", &price);
    menuPtr->prices[i] = price;
  }

  while (1) {
    printf(
        "1. Enter 1 to complete a waiter's shift.\n2. Enter 2 to close the "
        "restaurant.\n");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
      case 1: {
        int waiterNumber;
        printf("Enter the waiter number (from 1 to 10): ");
        scanf(" %d", &waiterNumber);
        totalProfit += completeShift(waiterNumber);
      } break;
      case 2: {
        closeRestaurant(menuId, menuPtr, totalProfit);
        exit(0);
      } break;
      default:
        printf("Invalid choice\n");
    }
  }
}

int completeShift(int waiterNumber) {
  int waiterId;
  int* waiterPtr;

  //   Attach to waiter shared memory.
  waiterId = shmget(waiterNumber + 200, sizeof(int), PERMS);
  if (waiterId == -1) {
    perror("Error in shmget");
    exit(1);
  }

  waiterPtr = (int*)shmat(waiterId, NULL, 0);
  if (waiterPtr == (void*)-1) {
    perror("Error in shmat");
    exit(1);
  }

  int sales = waiterPtr[0];
  waiterPtr[0] = sales / 5;
  printf("Paid %d to waiter %d\n", waiterPtr[0], waiterNumber);

  int profit = sales - waiterPtr[0];

  if (shmdt(waiterPtr) == -1) {
    perror("Error in shmdt");
    exit(1);
  }

  return profit;
}

void closeRestaurant(int menuId, struct Menu* menuPtr, int totalProfit) {
  printf("Total profit earned: %d\n", totalProfit);
  printf("Restaurant closed successfully\n");

  // Detach and delete Menu shared memory.
  if (shmdt(menuPtr) == -1) {
    perror("Error in shmdt");
    exit(1);
  }

  if (shmctl(menuId, IPC_RMID, 0) == -1) {
    perror("Error in shmctl");
    exit(1);
  }
}