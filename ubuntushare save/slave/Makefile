
CC = gcc
CCFLAGS = -std=gnu99 -pthread -lrt -D_POSIX_C_SOURCE=199309L -g -Wall

SRC = slave.c
EXE = slave

all: slave

%.o : %.c
	$(CC) -c $(CCFLAGS) $<

slave: slave.o network.o
	$(CC) $(CCFLAGS) -o $(EXE) slave.o  network.o

clean:
	rm -f core *.o slave

