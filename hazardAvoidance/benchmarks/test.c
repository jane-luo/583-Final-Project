#include <stdio.h>

int foo(int z){
    printf("%d", z);
    return 1;
}

int bar(int y){
    printf("%d", y);
    return 2;
}

int main()
{

  int in[1000]; 
  int i,j;

  foo(24);
  for (i = 0; i < 1000; i++)
  {
    in[i] = 0;
  }   

  for (j = 100; j < 1000; j++)
  {
   in[j]+= 10;
  }

  bar(2023);
  for (i = 0; i< 1000; i++)
    fprintf(stdout,"%d\n", in[i]);
  
  return 1;
}