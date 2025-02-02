#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THRESHOLD 1000  // Threshold for switching to sequential sorting

// Structure to hold arguments for the sorting function
typedef struct {
    int left;
    int right;
} Args;

int *arr;  // Global array

// Merge function to merge two sorted halves
void merge(int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

// Sequential Merge Sort (Fallback for small arrays)
void mergeSort(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(left, mid);
        mergeSort(mid + 1, right);
        merge(left, mid, right);
    }
}

// Parallel Merge Sort function
void *parallelMergeSort(void *arg) {
    Args *args = (Args *)arg;
    int left = args->left;
    int right = args->right;

    if (left < right) {
        // If the array size is small, switch to sequential sorting
        if (right - left < THRESHOLD) {
            mergeSort(left, right);
            return NULL;
        }

        int mid = left + (right - left) / 2;

        pthread_t leftThread, rightThread;
        Args leftArgs = {left, mid};
        Args rightArgs = {mid + 1, right};

        // Create threads for sorting the left and right halves
        pthread_create(&leftThread, NULL, parallelMergeSort, &leftArgs);
        pthread_create(&rightThread, NULL, parallelMergeSort, &rightArgs);

        // Wait for both threads to finish
        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);

        // Merge the sorted halves
        merge(left, mid, right);
    }
    return NULL;
}

// Main function
int main() {
    int n;
    printf("Enter number of elements: ");
    scanf("%d", &n);

    arr = (int *)malloc(n * sizeof(int));

    printf("Enter elements: ");
    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    Args args = {0, n - 1};
    pthread_t mainThread;

    // Create the first thread for parallel sorting
    pthread_create(&mainThread, NULL, parallelMergeSort, &args);
    pthread_join(mainThread, NULL);

    printf("Sorted array: ");
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    printf("\n");
    free(arr);  // Free allocated memory
    return 0;
}
