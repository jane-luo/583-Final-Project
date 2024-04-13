#include <stdio.h>

int main(){
  int A[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int B[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int i, j;
  j = 0;
  for(i = 0; i < 10; i++) {
      B[i] = A[j] * 23 + i;
      if(i % 8 == 0) 
        j = i;
      else
        j = i + 1;
    printf("%d\n", B[i]);
  }
  return 0;
}
