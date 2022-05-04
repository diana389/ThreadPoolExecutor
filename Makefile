CC = gcc
CFLAGS = -g -Wall -lm

all: build

build: main.c
	$(CC) -o main main.c $(CFLAGS)

clean:
	rm -f main
	rm -f *.o

