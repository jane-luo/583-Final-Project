#include <stdio.h>
#include <math.h>

double calculate_e(int terms) {
    double sum = 1.0;
    double factorial = 1.0;

    for (int i = 1; i <= terms; i++) {
        factorial *= i;
        sum += 1.0 / factorial;
    }

    return sum;
}

int main() {
    int num_terms = 100000000; // Increase this number for higher precision (and longer execution time)
    double e_approx = calculate_e(num_terms);

    printf("Approximation of e: %lf\n", e_approx);

    return 0;
}
