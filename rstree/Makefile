all: clean main donut_test

main: qsort.o poly34.o donut.o rstree_build.o rstree_search.o
	g++ -std=c++0x qsort.o poly34.o donut.o rstree_build.o rstree_search.o main.cpp -o main.out

donut_test: qsort.o poly34.o donut.o rstree_build.o rstree_search.o
	g++ -std=c++0x poly34.o donut.o donut_test.cpp -o donut_test.out

qsort.o:
	gcc -c qsort.c -o qsort.o

poly34.o:
	gcc -c poly34.c -o poly34.o

donut.o:
	gcc -c donut.c -o donut.o

rstree_search.o:
	gcc -c rstree_search.c -o rstree_search.o

rstree_build.o:
	gcc -c rstree_build.c -o rstree_build.o

clean:
	rm -f *.o
	rm -f *.out