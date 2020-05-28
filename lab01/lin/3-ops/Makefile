CC=gcc
FLAGS=-Wall

all: add.o mul.o ops.o
	$(CC) $^ -lm -o ops

add.o: add.c
mul.o: mul.c

ops.o: ops.c
	gcc -c -DHAVE_MATH -o ops.o ops.c

clean:
	rm -rf *.o ops

