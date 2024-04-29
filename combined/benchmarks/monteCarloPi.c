#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_POINTS 1000000000 // Increase this number for higher accuracy (and longer execution time)

double monte_carlo_pi(int num_points) {
    int inside_circle = 0;
    srand(time(NULL));

    for (int i = 0; i < num_points; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        double distance = x * x + y * y;

        if (distance <= 1) {
            inside_circle++;
        }
    }

    return 4.0 * inside_circle / num_points;
}

int main() {
    clock_t start = clock();
    double pi = monte_carlo_pi(NUM_POINTS);
    clock_t end = clock();

    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Approximation of pi: %lf\n", pi);
    printf("Time taken: %lf seconds\n", time_taken);

    return 0;
}