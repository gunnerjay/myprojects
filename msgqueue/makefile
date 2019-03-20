# specify the compiler
CC=gcc

# specify options for the compiler
CFLAGS= -Wall --std=gnu99 -g3

all: pmsg_send pmsg_recv pmsg_create

pmsg_send: pmsg_send.o
	$(CC) -o $@ $(CFLAGS) $^ -lpthread -lrt

pmsg_recv: pmsg_recv.o
	$(CC) -o $@ $(CFLAGS) $^ -lpthread -lrt

pmsg_create: pmsg_create.o
	$(CC) -o $@ $(CFLAGS) $^ -lpthread -lrt

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -rf *o pmsg_send pmsg_recv