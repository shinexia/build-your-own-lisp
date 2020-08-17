#include "status.h"

int is_status_ok(status_t* status) {
  return status == NULL || status->code == 0;
}
