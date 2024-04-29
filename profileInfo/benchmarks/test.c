// #include <stdio.h>

// int foo(int z){
//     printf("%d", z);
//     return 1;
// }

// int bar(int y){
//     printf("%d", y);
//     return 2;
// }

// int main()
// {

//   int in[1000]; 
//   int i,j;

//   foo(24);
//   for (i = 0; i < 1000; i++)
//   {
//     in[i] = 0;
//   }   

//   for (j = 100; j < 1000; j++)
//   {
//    in[j]+= 10;
//   }

//   bar(2023);
//   for (i = 0; i< 1000; i++)
//     fprintf(stdout,"%d\n", in[i]);
  
//   return 1;
// }

// #include <stdio.h>

// #define NUM_HEADERS 10000

// void compile_slow() {
//     // Generate a large number of #include statements programmatically
//     for (int i = 1; i <= NUM_HEADERS; i++) {
//         printf("#include \"header%d.h\"\n", i);
//     }
// }

// int main() {
//     compile_slow();

//     // Call some functions to avoid compiler optimizing everything out
//     printf("Compilation done!\n");

//     return 0;
// }



// #include <stdio.h>

// #define NUM_FUNCTIONS 10000

// void generate_code() {
//     printf("#include <stdio.h>\n\n");
//     printf("int main() {\n");
//     for (int i = 0; i < NUM_FUNCTIONS; i++) {
//         printf("    void function%d() {\n", i);
//         printf("        printf(\"Function %d\\n\");\n", i);
//         printf("    }\n");
//     }
//     printf("    // Call some of the generated functions\n");
//     for (int i = 0; i < NUM_FUNCTIONS; i++) {
//         if (i % 1000 == 0) {
//             printf("    function%d();\n", i);
//         }
//     }
//     printf("    return 0;\n");
//     printf("}\n");
// }

// int main() {
//     generate_code();
//     return 0;
// }



// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>

// #define NUM_POINTS 1000000000 // Increase this number for higher accuracy (and longer execution time)

// double monte_carlo_pi(int num_points) {
//     int inside_circle = 0;
//     srand(time(NULL));

//     for (int i = 0; i < num_points; i++) {
//         double x = (double)rand() / RAND_MAX;
//         double y = (double)rand() / RAND_MAX;
//         double distance = x * x + y * y;

//         if (distance <= 1) {
//             inside_circle++;
//         }
//     }

//     return 4.0 * inside_circle / num_points;
// }

// int main() {
//     clock_t start = clock();
//     double pi = monte_carlo_pi(NUM_POINTS);
//     clock_t end = clock();

//     double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

//     printf("Approximation of pi: %lf\n", pi);
//     printf("Time taken: %lf seconds\n", time_taken);

//     return 0;
// }


// #include <stdio.h>
// #include <math.h>

// double calculate_e(int terms) {
//     double sum = 1.0;
//     double factorial = 1.0;

//     for (int i = 1; i <= terms; i++) {
//         factorial *= i;
//         sum += 1.0 / factorial;
//     }

//     return sum;
// }

// int main() {
//     int num_terms = 100000000; // Increase this number for higher precision (and longer execution time)
//     double e_approx = calculate_e(num_terms);

//     printf("Approximation of e: %lf\n", e_approx);

//     return 0;
// }



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
