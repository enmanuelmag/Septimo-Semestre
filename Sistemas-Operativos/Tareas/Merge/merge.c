#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF 20000
#define LEN_BUFF 1000

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    /* create temp arrays */
    int L[n1], R[n2];
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 
/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = (l+r)/2;
 
        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
 
        merge(arr, l, m, r);
    }
}

/* Function to print an array */
void printArray(int A[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

int main(int argc, char *argv[])
{
    char ch[BUFF];
    char ch2[BUFF];
    strcpy(ch2, argv[1]);
    int size = 0;
    printf("%s\n", ch2);
    char *b = strtok(ch2, ",");
    while (b != NULL)
    {
        size++;
        b = strtok(NULL, ",");
    }
    
    int *numbers = (int *)calloc(size, sizeof(int));
    char *p = strtok(argv[1], ",");
    int i = 0;
    while (p != NULL)
    {
        numbers[i] = atoi(p);
        p = strtok(NULL, ",");
        i++;
    }

    mergeSort(numbers, 0, size - 1);

    printArray(numbers, size);

    free(numbers);

    return 0;
}