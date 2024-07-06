#ifndef UTILS_H
#define UTILS_H

#define PERMS 0644
#define PATHNAME "./src/manager.c"
#define PROJ_ID 'C'
#define MESSSAGE_DELIMITER ";"
#define MAX_ORDER_SIZE 50

struct TableOrder {
  int tableNumber;
  int numberOfOrders;
  int orders[MAX_ORDER_SIZE];
};

struct Bill {
  int numberOfItems;
  int total;
};

struct Menu {
  int numberOfItems;
  int prices[MAX_ORDER_SIZE];
};

#endif