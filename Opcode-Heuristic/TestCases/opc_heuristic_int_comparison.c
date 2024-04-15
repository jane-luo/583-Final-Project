#include <stdio.h>

int main() {
    int a = -5;
    int b = 0;
    int c = 5;

    // Check negative number less than zero
    if (a < 0) {
        printf("Negative less than zero: Expected to be true.\n");
    } else {
        printf("Negative less than zero: Unexpected false.\n");
    }

    // Check zero less than zero (should not happen)
    if (b < 0) {
        printf("Zero less than zero: Unexpected true.\n");
    } else {
        printf("Zero less than zero: Expected to be false.\n");
    }

    // Check positive number less than zero (should not happen)
    if (c < 0) {
        printf("Positive less than zero: Unexpected true.\n");
    } else {
        printf("Positive less than zero: Expected to be false.\n");
    }

    return 0;
}
