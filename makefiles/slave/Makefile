
CC = gcc
CCFLAGS =  -g -Wall -pthread
#CCFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
EXE = slave

SRCS = network.c slave.c

all: app

app:
        $(CC) $(CCFLAGS) $(SRCS) -o $(EXE)

clean:
        rm -f core *.o slave