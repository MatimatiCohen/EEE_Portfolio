
hw5:  hw5.o utils.o filesys.o
	gcc utils.o filesys.o hw5.o -o hw5

utils.o: utils.h

filesys.o: filesys.h

.PHONY: all clean test

all: hw5

clean:
	-rm *.o hw5
