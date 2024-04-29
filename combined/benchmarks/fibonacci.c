#include <stdio.h>

#define RECURSION_DEPTH 45

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main() {
    int result = fibonacci(RECURSION_DEPTH);
    printf("Result: %d\n", result);
    return 0;
}
