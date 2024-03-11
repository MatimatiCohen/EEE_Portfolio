#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include "pass1.h"
#include "pass2.h"

// TODO: use the given values in the guidelines
#define MAX_ROW_SIZE 500
#define MAX_LABEL 50
#define MAX_ROWS_NUM 200


void main(int argc, char* argv[]) {
	
	//printf("%s\n", argv[0]);
	//printf("%s\n", argv[1]);

	FILE* f = NULL;
	char* memin = NULL;
	label labels_array[MAX_ROWS_NUM];
	int label_count = 0;       // The amount of labels in the file
	int out_lines_num = 0;    // The number of lines in the output file

	// TODO: Check if the arguments are valid
	// Open the file
	fopen_s(&f, argv[1], "r");
	assert(f != NULL);
	
	// Fill the names and coresponding locations of every label
	create_labels_array(f, labels_array, &label_count, &out_lines_num);

	// Fill the memim file with the machine language instructions
	second_pass(f, argv, labels_array, label_count);

	fclose(f);
}



