#include "utils.h"

//Parse Data functions
int GetArgs(ConfField* conf_fields, UserConf* user_conf, Mem* mem, int argc, char** args)
{
	if (argc != NUM_OF_ARGS) return 0;
	FILE* config_file;
	FILE* mem_in_file;

	config_file = fopen(args[CONFIG_FILE_ARG], "r");
	if (config_file == NULL) return 0;
	SetConfig(conf_fields, user_conf, config_file);

	mem_in_file = fopen(args[MEMIN_FILE_ARG], "r");
	if (mem_in_file == NULL) return 0;
	InitMem(mem, mem_in_file);

	return 1;
}

//User Configurations Functions
void SetConfig(ConfField* conf_fields, UserConf* user_conf, FILE *config_file)
{
	int num_pairs = 0;
	char *delim = " = ";

	while (num_pairs < NUM_CONF_FIELDS) {
		char* line = NULL;
		size_t lineSize = 0;
		size_t charsRead = getline(&line, &lineSize, config_file); //read file line by line

		if (charsRead) {
			//split to parmeter name and value
			char* pos = strstr(line, delim);
			if (pos != NULL) {
				char* value_str = pos + strlen(delim);
				strncpy(conf_fields[num_pairs].value_str,value_str, MAX_CONF_LINE_LEN);
				*pos = '\0';
			}

			strncpy(conf_fields[num_pairs].parameter, line, MAX_CONF_LINE_LEN);
			num_pairs++;
		}
	}

	//covert from text to config field
	for (int i = 0; i < NUM_CONF_FIELDS; i++) {
		if (!strcmp(conf_fields[i].parameter, "add_nr_units")) {
			user_conf->add_nr_units = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "sub_nr_units")) {
			user_conf->sub_nr_units = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "mul_nr_units")) {
			user_conf->mul_nr_units = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "div_nr_units")) {
			user_conf->div_nr_units = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "ld_nr_units")) {
			user_conf->ld_nr_units = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "st_nr_units")) {
			user_conf->st_nr_units = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "add_delay")) {
			user_conf->add_delay = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "sub_delay")) {
			user_conf->sub_delay = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "mul_delay")) {
			user_conf->mul_delay = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "div_delay")) {
			user_conf->div_delay = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "ld_delay")) {
			user_conf->ld_delay = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "st_delay")) {
			user_conf->st_delay = strtol(conf_fields[i].value_str, NULL, 10);
		}
		if (!strcmp(conf_fields[i].parameter, "trace_unit")) {
			strcpy(user_conf->trace_unit, conf_fields[i].value_str);
		}
	}
}

//Pure Utils
size_t getline(char** lineptr, size_t* n, FILE* stream) {
	char* bufptr = NULL;
	char* p = bufptr;
	size_t size;
	int c;

	if (lineptr == NULL) {
		return -1;
	}
	if (stream == NULL) {
		return -1;
	}
	if (n == NULL) {
		return -1;
	}
	bufptr = *lineptr;
	size = *n;

	c = fgetc(stream);
	if (c == EOF) {
		return -1;
	}
	if (bufptr == NULL) {
		bufptr = malloc(128);
		if (bufptr == NULL) {
			return -1;
		}
		size = 128;
	}
	p = bufptr;
	while (c != EOF) {
		if ((p - bufptr) > (size - 1)) {
			size = size + 128;
			bufptr = realloc(bufptr, size);
			if (bufptr == NULL) {
				return -1;
			}
		}
		*p++ = c;
		if (c == '\n') {
			break;
		}
		c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;
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

int hex_to_char(int dec)
{
	if (dec < 10)
		return '0' + dec;
	return dec + 'a' - 10;
}