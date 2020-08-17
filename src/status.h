#ifndef STATUS_H
#define STATUS_H

#include "stddef.h"

typedef struct status {
  int code;
  char message[80];
} status_t;

int is_status_ok(status_t* status);

#endif  // !STATUS_H
