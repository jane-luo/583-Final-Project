#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = NULL;
    int *allocated = malloc(sizeof(int));

    // Expect heuristic to consider NULL checks frequent
    if (ptr == NULL) {
        printf("Pointer is NULL (expected).\n");
    } else {
        printf("Pointer is not NULL (unexpected).\n");
    }

    // Should be unlikely for allocated to be NULL
    if (allocated == NULL) {
        printf("Allocated pointer is NULL (unexpected).\n");
    } else {
        printf("Allocated pointer is not NULL (expected).\n");
    }

    free(allocated);
    return 0;
}
