
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool msg_has_substr(const char *msg, int msg_len, const char *sub_str);

int create_random_port_number(int min, int max)
{
  assert(max >= min);
  int range = max - min + 1;
  int random_int = rand();
  random_int %= range;
  int port_number = random_int + min;
  return port_number;
}

void write_port_to_file(int port_num, const char *file_name)
{
  FILE *file_pointer = NULL;
  file_pointer = fopen(file_name, "w");
  assert(file_pointer != NULL);
  fprintf(file_pointer, "%d\n", port_num);
  fclose(file_pointer);
}

int count_substr_in_msg(const char *msg, int msg_len, const char *sub_str)
{
  int i;
  int counter = 0;
  for (i = 0; i < msg_len; i++) {
    if (msg_has_substr(msg + i, msg_len - i, sub_str)) {
      counter++;
    }
  }
  return counter;
}

bool msg_has_substr(const char *msg, int msg_len, const char *sub_str)
{
  int i;
  int str_len = strlen(sub_str);
  for (i = 0; i < str_len; i++) {
    if (i >= msg_len) {
      return false;
    }
    if (sub_str[i] != msg[i]) {
      return false;
    }
  }
  return true;
}
