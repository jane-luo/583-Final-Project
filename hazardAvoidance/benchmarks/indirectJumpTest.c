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
