#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "user_actions.h"
#include "line_handler.h"
#include <unistd.h>



FILE* open_file_for_read(FILE *f,user_actions *actions);

int main(int argc, char *argv[]){
  user_actions actions;
  FILE *stream= NULL;
  initialize_actions(&actions);
  if (!isatty(STDIN_FILENO)){
      stream=stdin;
      actions.file_name=argv[0];
      read_user_actions(argc,argv,&actions);
  }
  else {
      read_user_actions(argc, argv, &actions);
      stream = open_file_for_read(stream, &actions);
  }
  char *line = NULL;
  while (read_line(stream,&line)!=EOF){
      actions.line_count++;
      is_match_in_line(line,&actions);
      actions.bytes_count= ftell(stream);
      free(line);
  }
  free(line);
  if ((actions.c.is_on)&&!(actions.v.is_on)) {
      printf("%d\n", actions.valid_rows_num);
  }
  else if((actions.c.is_on)&&(actions.v.is_on)) {
      printf("%d\n", actions.non_valid_rows_num);
  }
  fclose(stream);
  return 0;
}



FILE* open_file_for_read(FILE *f,user_actions *actions){
    f = fopen(actions->file_name, "r");
    assert(f != NULL);
    return f;
}
