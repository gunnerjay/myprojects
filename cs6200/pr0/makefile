# specify the compiler
CC=g++

# specify options for the compiler
CFLAGS=-c -Wall -g

all: hello

hello: main.o
	$(CC) main.o -o hello.exe

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

#hello.o: hello.cpp
#    $(CC) $(CFLAGS) hello.cpp

clean:
	rm -rf *o hello.exe