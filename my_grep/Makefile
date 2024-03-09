my_grep:  my_grep.o user_actions.o flag_of_action.o string_checks.o line_handler.o  reg_exp.o
	gcc my_grep.o user_actions.o flag_of_action.o string_checks.o line_handler.o  reg_exp.o -o my_grep

line_handler.o: line_handler.h

user_actions.o: user_actions.h

flag_of_action.o: flag_of_action.h

reg_exp.o: reg_exp.h

string_checks.o: string_checks.h

.PHONY: all clean test

all: my_grep


clean:
	-rm *.o my_grep


test: all
			./run_all.sh
