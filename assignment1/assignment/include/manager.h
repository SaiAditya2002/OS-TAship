#ifndef MANAGER_H
#define MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

int completeShift(int waiterNumber);
void closeRestaurant(int menuId, struct Menu* menuPtr, int totalProfit);

#endif