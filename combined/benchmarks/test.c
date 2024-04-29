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

void bubbleSort(int arr[], int n) {
    int i, j, temp;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                // Swap arr[j] and arr[j+1]
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

void printArray(int arr[], int size) {
    int i;
    for (i=0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(arr)/sizeof(arr[0]);
    
    printf("Unsorted array: \n");
    printArray(arr, n);
    
    bubbleSort(arr, n);
    
    printf("Sorted array: \n");
    printArray(arr, n);
    
    return 0;
}

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