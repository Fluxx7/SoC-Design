
fcc: compiler.o computils.o preprocessor.o targets.o
	gcc -std=c2x -g -Wall compiler.o computils.o preprocessor.o targets.o -lm -o fcc




compiler.o: src/compiler.c src/computils.h src/preprocessor.h targets/targets.h targets/targetdef.h
	gcc -std=c2x -g -Wall -c src/compiler.c

computils.o: src/computils.c src/computils.h 
	gcc -std=c2x -g -Wall -c src/computils.c

preprocessor.o: src/preprocessor.c src/preprocessor.h src/computils.h targets/targetdef.h
	gcc -std=c2x -g -Wall -c src/preprocessor.c

targets.o: targets/ng-core/ng-core.c targets/ng-core/ng-core.h targets/targetdef.h targets/ng-core/ng-core-defs.h targets/ng-core/ng-core-target.h src/computils.h
	gcc -std=c2x -g -Wall -c targets/ng-core/ng-core.c -o targets.o








# various removal functions

clean:
	rm -f compiler.o computils.o fcc testout.txt mirror.txt

c_opts:
	rm -f mirror.txt

c_part:
	rm -f compiler.o computils.o preprocessor.o targets.o

all: fcc