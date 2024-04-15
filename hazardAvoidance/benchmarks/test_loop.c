#include <stdio.h>

int main(int argc, char *argv[]) {
    int a = 2;
    int arr[1];
    if (a == 2) {
        for (int i = 1; i <= 100000; i++) {
            for (int j = 0; j <= 50000; j++) {

                arr[0] = j;
            }
        }
    } else if (a == 3) {
        a++;
    } else {
        a--;
    }
    return 0;
}