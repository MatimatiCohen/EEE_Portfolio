#ifndef UTILS_H
#define UTILS_H

int create_random_port_number(int min, int max);
void write_port_to_file(int port_num, const char* file_name);
int count_substr_in_msg(const char* msg, int msg_len, const char* sub_str);

#endif
