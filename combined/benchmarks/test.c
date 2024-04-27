// #include <stdio.h>

// int main(){
//   int A[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//   int B[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//   int i, j;
//   j = 0;
//   for(i = 0; i < 10; i++) {
//       B[i] = A[j] * 23 + i;
//       if(i % 8 == 0) 
//         j = i;
//       else
//         j = i + 1;
//     printf("%d\n", B[i]);
//   }
//   return 0;
// }

#include <stdio.h>
#include <stdbool.h>

void A() {
    printf("Inside label A\n");
}

void B() {
    printf("Inside label B\n");
}

void jump(bool condition, void (*target)()) {
    if (condition) {
        printf("Condition is true\n");
        target();
    } else {
        printf("Condition is false\n");
        // Indirect jump
        void *targets[] = {&&target_A, &&target_B};
        goto *targets[0];
    }

target_A:
    A();
    return;

target_B:
    B();
    return;
}

int main() {
    bool condition = true;  // Set the condition to true
    jump(condition, &A);
    return 0;
}