CC=gcc
CFLAGS=-Wall -g -O3
INCLUDES=PriorityQueue.h

OBJECTS=PriorityQueue.o AKnight.o

all: ${OBJECTS} ${INCLUDES}
	gcc -o search ${OBJECTS}

