#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr1 = malloc(sizeof(int));
    int *ptr2 = ptr1;  // Pointers expected to be equal
    int *ptr3 = malloc(sizeof(int));

    // Expected: ptr1 == ptr2 is true; hence heuristic should favor this path if checking for equality
    if (ptr1 == ptr2) {
        printf("Pointers are equal (as expected).\n");
    } else {
        printf("Pointers are not equal (unexpected).\n");
    }

    // Expected: ptr1 != ptr3 is true; heuristic should favor this path if checking for inequality
    if (ptr1 != ptr3) {
        printf("Pointers are not equal (as expected).\n");
    } else {
        printf("Pointers are equal (unexpected).\n");
    }

    free(ptr1);
    free(ptr3);
    return 0;
}
