#include "pass1.h"
#include "pass2.h"
#include <string.h>


// First pass  - update all the labels in the lables_array
void create_labels_array(FILE* f, label labels_array[], int* label_count, int* out_lines_num)
{
	// variables
	char line[MAX_ROW_SIZE];
	char label[MAX_LABEL];
	int pc = 0;
	int line_num = -1; // to use for debug
	int labels_idx = 0;
	command cmd; 
	
	// Loop over all lines in the assembly file
	while (fgets(line, sizeof(line), f) != NULL) {

		
		line_num++;
		//printf("\nline %d is corresponds to pc %d\n", line_num, pc);
		errno_t name_buffer = 50;
		errno_t adrs_buffer = 6;

		// returns true in case there is a lable in the line
		int labeled = read_label_from_line(line, label);
		// label_and_command is an indicator of the case of a command and a label in the same line
		int label_and_command = parse_line(line, &cmd, labels_array, 1,label_count);

		if (!check_line_valid(line, 0)) {
			//printf("\n------------------------------\n");
			//printf("line %d is commented or empty!\n",line_num);
			//printf("------------------------------\n");
			continue;
		}

		if (labeled){

			*label_count = *label_count + 1;
			char hex_address[ADDRESS_LEN + 1];
			// Convert the Address to hex character
			dec_to_n_chars_hex(hex_address, pc, ADDRESS_LEN);

			// Update the labels_array's fields
			strcpy_s(labels_array[labels_idx].name, name_buffer, label);
			strcpy_s(labels_array[labels_idx].address, adrs_buffer, hex_address);
			
			//printf("hex adress = %s\n", labels_array[labels_idx].address);
			//printf("int adress = %d\n\n", pc);
			labels_idx += 1;

			if (label_and_command) {
				if (!strcmp(cmd.type, "O")) {
					continue;
				}
				else if (is_cmd_i_type(&cmd))
				{
					pc += 2;
					continue;
				}
				// If the command is translated to one line
				pc++;
			}

			continue;
		}
		// If the command is a .word command
		else if (!strcmp(cmd.type, "O")) {
			continue;
		}
		// If the command is an I command 
		else if (is_cmd_i_type(&cmd))
		{
			pc += 2;
			continue;
		}
		// If the command is translated to one line
		pc++;
	}
	*out_lines_num = pc;
	//printf("\n%d\n\n\n", *out_lines_num);

	return;
}


//void get_first_word

// Read the label, if exists, from a given line
bool read_label_from_line(char* line,char* label)
{	
	int line_index = 0;
	int label_index = 0;
	
	// label_index holds the first index of the first word in the command
	while (line[line_index] == ' ' || line[line_index] == '\n' || line[line_index] == '\t') {
		line_index++;
	}
	label_index = line_index;
	while (line[line_index] != ':')
	{
		// In case the first word didn't end with ':'
		if (line[line_index] == ' ' || line[line_index] == ',' || line[line_index] == '\t')
		{
			return false;
		}
		line_index++;
	}
	// In case the word ended with ':', return the label
	if (line[line_index] == ':')
	{
		int i = 0;
		while (line[label_index] != ':')
		{
			label[i] = line[label_index];
			label_index++;
			i++;
		}
		label[i] = '\0';
		return true;
	}
	return false;
}

// Check if the line is commented or empty
bool check_line_valid(char* line, int line_index)
{
	// skip the spaces at the beggining of the line
	while (isspace(line[line_index])) line_index++;

	if (line[line_index] == '#' || line[line_index] == '\0' || line[line_index] == '\n')
	{
		return false;
	}
	return true;
}

int hex_to_char(int dec)
{
	if (dec < 10)
		return '0' + dec;
	return dec + 'A' - 10;
}

// Convert a decimal number to an n-sized char in hex 
void dec_to_n_chars_hex(char* hex_char, int dec, int n)
{
	int l_dec = dec;
	int l_val = 0;
	unsigned int u_l_dec = dec;

	// for negative values
	if (dec >= 0)
	{
		for (int j = n - 1; j >= 0; j--)
		{
			l_val = l_dec % 16;
			hex_char[j] = (char)hex_to_char(l_val);
			l_dec /= 16;
		}
	}

	//convering negative number to hex using unsigned int
	else
	{
		for (int j = n - 1; j >= 0; j--)
		{
			l_val = u_l_dec % 16;
			hex_char[j] = (char)hex_to_char(l_val);
			u_l_dec /= 16;
		}
	}

	hex_char[n] = '\0';
}
