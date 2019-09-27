all: clean main

main: qsort.o rstree_build.o rstree_search.o
	g++ -std=c++0x qsort.o rstree_build.o rstree_search.o main.cpp -o main.out

qsort.o:
	gcc -c qsort.c -o qsort.o

rstree_search.o:
	gcc -c rstree_search.c -o rstree_search.o

rstree_build.o:
	gcc -c rstree_build.c -o rstree_build.o

clean:
	rm -f *.o
	rm -f *.out