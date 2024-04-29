#include <stdio.h>

int factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    else
        return n * factorial(n - 1);
}

int main() {
    int num, fact;
    
    printf("Enter a number: ");
    scanf("%d", &num);
    
    fact = factorial(num);
    
    printf("Factorial of %d is %d\n", num, fact);
    
    return 0;
}
