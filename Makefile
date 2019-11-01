
CC = gcc
CCFLAGS = -std=gnu99 -pthread -lrt -D_POSIX_C_SOURCE=199309L -g -Wall

SRC = master.c
EXE = master

all: master

%.o : %.c
	$(CC) -c $(CCFLAGS) $<

master: master.o network.o
	$(CC) $(CCFLAGS) -o $(EXE) master.o  network.o

clean:
	rm -f core *.o master

