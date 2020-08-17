#include "util.h"

#include <stdio.h>
#include <string.h>

int str_startswith(const char* str, const char* prefix) {
  size_t lenpre = strlen(prefix), lenstr = strlen(str);
  return lenstr < lenpre ? 0 : memcmp(prefix, str, lenpre) == 0;
}
