.PHONY: a.out
a.out:
	gcc -c -pthread ThreadPool.c
	gcc -pthread -o test test.c ThreadPool.o
