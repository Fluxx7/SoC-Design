
ngcc: compiler.o computils.o
	gcc -std=c2x -g -Wall compiler.o computils.o -lm -o ngcc

compiler.o: compiler.c compiler.h computils.h
	gcc -std=c2x -g -Wall -c compiler.c

computils.o: computils.c computils.h
	gcc -std=c2x -g -Wall -c computils.c

clean:
	rm -f compiler.o computils.o ngcc testout.txt mirror.txt

c_opts:
	rm -f mirror.txt

c_part:
	rm -f compiler.o computils.o

all: ngcc