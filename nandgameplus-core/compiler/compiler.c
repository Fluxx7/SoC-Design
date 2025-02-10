#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    while (1) {
        char line[256];
        //getline(&line, 256, input);

    }
    fclose(input);
    fclose(output);
    return 0;
}