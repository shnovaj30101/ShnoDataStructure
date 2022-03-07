
CC=gcc
CFLAGS=-Wall -g

%.o: %.c
	$(CC) $(CFLAGS) -c $^

all: myrandom.o mytree.o
	$(CC) $(CFLAGS) main.c myrandom.o mytree.o -o main

debug1: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DDEBUG1 main.c myrandom.o mytree.o -o main

debug2: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DDEBUG2 main.c myrandom.o mytree.o -o main

debug3: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DDEBUG3 main.c myrandom.o mytree.o -o main

clean:
	rm *.o *.so *.a
