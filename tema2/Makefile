build: so_stdio.o
	gcc -std=c89 -shared so_stdio.o -o libso_stdio.so

so_stdio.o:
	gcc -std=c89 -fPIC -c so_stdio.c

run: clean build
	gcc -std=c89 test.c -o test -lso_stdio -L.
	./test

clean:
	rm -f *.so *.o