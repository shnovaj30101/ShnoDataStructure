
CC=gcc
CFLAGS=-Wall -g

%.o: %.c
	$(CC) $(CFLAGS) -c $^

all: myrandom.o mytree.o
	$(CC) $(CFLAGS) main.c myrandom.o mytree.o -o main

check: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DCHECK main.c myrandom.o mytree.o -o main

debug1: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DDEBUG1 -DCHECK main.c myrandom.o mytree.o -o main

debug2: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DDEBUG2 -DCHECK main.c myrandom.o mytree.o -o main

debug3: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DDEBUG3 -DCHECK main.c myrandom.o mytree.o -o main

debug4: myrandom.o mytree.o
	$(CC) $(CFLAGS) -DDEBUG4 -DCHECK main.c myrandom.o mytree.o -o main

clean:
	rm *.o *.so *.a
