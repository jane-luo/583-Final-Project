#include <stdio.h>

int main() {
    double x = 0.1;
    double y = 0.1;
    double z = 0.1 + 0.2; // Not exactly 0.3 due to floating-point arithmetic

    // Check exact equality (likely true in this artificial example)
    if (x == y) {
        printf("Floating point equality (x == y): Expected to be true.\n");
    } else {
        printf("Floating point equality (x == y): Unexpected false.\n");
    }

    // Check inexact equality (typical floating-point issue)
    if (x == z) {
        printf("Floating point equality (x == z): Unexpected true.\n");
    } else {
        printf("Floating point equality (x == z): Expected to be false.\n");
    }

    return 0;
}
