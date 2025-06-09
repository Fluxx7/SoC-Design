
fcc: src/compiler.c src/utilities/computils.h src/utilities/computils.c targets/targets.h targets/targetdef.h src/preprocessor/preprocessor.c src/preprocessor/preprocessor.h src/preprocessor/preprocessor_utils.h src/preprocessor/preprocessor_utils.c targets/ng-core/ng-core.c targets/ng-core/ng-core.h targets/ng-core/ng-core-defs.h targets/ng-core/ng-core-target.h 
	gcc -std=c2x -g -gdwarf-4 -Wall src/compiler.c src/utilities/computils.c src/preprocessor/preprocessor.c src/preprocessor/preprocessor_utils.c targets/ng-core/ng-core.c -lm -o fcc

fccpart: compiler.o computils.o preprocessor.o targets.o
	gcc -std=c2x -g -Wall compiler.o computils.o preprocessor.o targets.o -lm -o fcc

compiler.o: src/compiler.c src/utilities/computils.h src/preprocessor/preprocessor.h targets/targets.h targets/targetdef.h
	gcc -std=c2x -g -Wall -c src/compiler.c

computils.o: src/utilities/computils.c src/utilities/computils.h 
	gcc -std=c2x -g -Wall -c src/utilities/computils.c

preprocessor.o: src/preprocessor/preprocessor.c src/preprocessor/preprocessor.h src/preprocessor/preprocessor_utils.h src/preprocessor/preprocessor_utils.c src/utilities/computils.h targets/targetdef.h
	gcc -std=c2x -g -Wall -c src/preprocessor/preprocessor.c src/preprocessor/preprocessor_utils.c

targets.o: targets/ng-core/ng-core.c targets/ng-core/ng-core.h targets/targetdef.h targets/ng-core/ng-core-defs.h targets/ng-core/ng-core-target.h src/utilities/computils.h
	gcc -std=c2x -g -Wall -c targets/ng-core/ng-core.c -o targets.o








# various removal functions

clean:
	rm -f compiler.o computils.o fcc testout.txt mirror.txt

c_opts:
	rm -f mirror.txt

c_part:
	rm -f compiler.o computils.o preprocessor.o targets.o

all: fcc