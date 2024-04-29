#include <stdio.h>

#define NUM_HEADERS 10000

void compile_slow() {
    // Generate a large number of #include statements programmatically
    for (int i = 1; i <= NUM_HEADERS; i++) {
        printf("#include \"header%d.h\"\n", i);
    }
}

int main() {
    compile_slow();

    // Call some functions to avoid compiler optimizing everything out
    printf("Compilation done!\n");

    return 0;
}
