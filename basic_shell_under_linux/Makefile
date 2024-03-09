run_shell:  run_shell.o shell.o utils.o jobs.o
	gcc run_shell.o shell.o utils.o jobs.o -o run_shell

shell.o: shell.h

utils.o: utils.h

jobs.o: jobs.h

.PHONY: all clean test

all: run_shell


clean:
	-rm *.o run_shell

