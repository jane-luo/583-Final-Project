#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr1 = malloc(sizeof(int));
    int *ptr2 = NULL;  // ptr2 is explicitly set to NULL
    int *ptr3 = malloc(sizeof(int));
    *ptr1 = 10;
    *ptr3 = 30;

    // Test 1: Guarding against NULL (checking for non-NULL)
    if (ptr2 != NULL) {
        printf("ptr2 is not NULL (unexpected).\n");
    } else {
        // expected
        printf("ptr2 is NULL (expected).\n");
    }

    // Test 2: Using the variable as LHS in branch
    if (ptr1 != NULL) {
        // expected
        printf("ptr1 is not NULL, safe to use: %d (expected).\n", *ptr1);
    } else {
        printf("ptr1 is NULL (unexpected).\n");
    }

    // Test 3: Double guarding with a used operand
    if (ptr3 != NULL && *ptr3 == 30) {
        // expected
        printf("ptr3 is not NULL and points to 30, safe to use (expected).\n");
    } else {
        printf("ptr3 failed checks (unexpected).\n");
    }

    free(ptr1);
    free(ptr3);
    return 0;
}
