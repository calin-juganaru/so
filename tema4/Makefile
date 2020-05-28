CC = gcc
CFLAGS = -fPIC -Wall

.PHONY: build
build: libscheduler.so

libscheduler.so: scheduler.o so_scheduler.h
	$(CC) $(CFLAGS) -shared -o $@ $^

scheduler.o: scheduler.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f libscheduler.so scheduler.o
