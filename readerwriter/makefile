# specify the compiler
CC=gcc

# specify options for the compiler
CFLAGS= -Wall --std=gnu99 -g3

all: thread

thread: main.o steque.o
	$(CC) -o $@ $(CFLAGS) $^ -lpthread

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -rf *o thread