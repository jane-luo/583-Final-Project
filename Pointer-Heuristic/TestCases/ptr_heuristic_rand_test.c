#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    int *ptr1 = (rand() % 2) ? malloc(sizeof(int)) : NULL;
    int *ptr2 = (rand() % 2) ? malloc(sizeof(int)) : NULL;

    // Random comparisons, heuristic utility less clear
    if (ptr1 == ptr2) {
        printf("Random pointers are equal.\n");
    } else {
        printf("Random pointers are not equal.\n");
    }

    free(ptr1);
    free(ptr2);
    return 0;
}
