CC = gcc
CPPFLAGS = -I../utils -Wno-unused-variable
CFLAGS = -Wall -Wextra -g

.PHONY: all clean

all: epoll

epoll: epoll.o

epoll.o: epoll.c ../utils.h

clean:
	-rm -f *.o *~ epoll *.txt
