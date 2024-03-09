
hw2:  dispatcher.o utils.o commands.o shared_work_queue.o
	gcc -pthread -g -Wall dispatcher.o utils.o commands.o shared_work_queue.o -o hw2

commands.o: commands.h

utils.o: utils.h

shared_work_queue.o: shared_work_queue.h

.PHONY: all clean test

all: hw2

clean:
	-rm *.o count**.txt thread**.txt dispatcher.txt stats.txt hw2 commands shared_work_queue dispatcher
