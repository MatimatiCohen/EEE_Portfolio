#ifndef PASS1_H
#define PASS1_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_ROW_SIZE 500
#define MAX_LABEL 50
#define MAX_ROW 500
#define ADDRESS_LEN 5

// A struct for holding a label
typedef struct Label {
	char  name[MAX_LABEL];
	char  address[ADDRESS_LEN+1];
}label;

//-------------------------Declerations-------------------------------//
void create_labels_array(FILE* f, label labels_array[], int* label_count, int* out_lines_num);
bool check_line_valid(char* line, int line_index);
bool read_label_from_line(char* line, char* label);
int hex_to_char(int dec);
void dec_to_n_chars_hex(char* hex_char, int dec, int n);
//--------------------------------------------------------------------//

#endif 