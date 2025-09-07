#include <string.h>

int strncmp(char *arg1, char *arg2, int size) {
  for (int i = 0; i < size; i ++)
    if (arg1[i] != arg2[i])
      return 1;
  return 0;
}
