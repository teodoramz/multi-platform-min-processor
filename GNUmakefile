CC=gcc
CFLAGS=-Wall -g -c

build: so-cpp

so-cpp: source.o hashMap.o listNode.o
	$(CC) -Wall -o so-cpp source.o hashMap.o listNode.o

source.o : source.c hashMap.h
	$(CC) $(CFLAGS) source.c -o source.o

hashMap.o : hashMap.c hashMap.h
	$(CC) $(CFLAGS) hashMap.c -o hashMap.o

listNode.o : listNode.c listNode.h
	$(CC) $(CFLAGS) listNode.c -o listNode.o

clean:
	rm *.o so-cpp
