#include <stdio.h>

#define MAX_SIZE 100

typedef struct {
    int arr[MAX_SIZE];
    int size;
} Heap;

// Function to swap two values
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify function (used for deletion)
void heapify(Heap *h, int i) {
    int largest = i;
    int left = 2 * i + 1;  // Left child index
    int right = 2 * i + 2; // Right child index

    // If left child is greater than root
    if (left < h->size && h->arr[left] > h->arr[largest])
        largest = left;

    // If right child is greater than largest so far
    if (right < h->size && h->arr[right] > h->arr[largest])
        largest = right;

    // If largest is not root
    if (largest != i) {
        swap(&h->arr[i], &h->arr[largest]);
        heapify(h, largest);
    }
}

// Insert into Max-Heap
void insert(Heap *h, int value) {
    if (h->size == MAX_SIZE) {
        printf("Heap Overflow!\n");
        return;
    }

    // Insert the new value at the end
    int i = h->size;
    h->arr[i] = value;
    h->size++;

    // Fix the max heap property if it's violated
    while (i > 0 && h->arr[(i - 1) / 2] < h->arr[i]) {
        swap(&h->arr[i], &h->arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Extract (Remove) Max Element
int extractMax(Heap *h) {
    if (h->size <= 0) {
        printf("Heap Underflow!\n");
        return -1;
    }
    if (h->size == 1) {
        h->size--;
        return h->arr[0];
    }

    // Store the maximum value and remove it
    int root = h->arr[0];
    h->arr[0] = h->arr[h->size - 1];
    h->size--;

    // Heapify the root element to maintain heap property
    heapify(h, 0);

    return root;
}

// Print the heap array
void printHeap(Heap *h) {
    printf("Heap: ");
    for (int i = 0; i < h->size; i++)
        printf("%d ", h->arr[i]);
    printf("\n");
}

int main() {
    Heap h;
    h.size = 0;

    insert(&h, 50);
    insert(&h, 30);
    insert(&h, 40);
    insert(&h, 10);
    insert(&h, 5);
    insert(&h, 20);
    insert(&h, 30);

    printHeap(&h);

    printf("Extracted Max: %d\n", extractMax(&h));
    printHeap(&h);

    return 0;
}

//functions for code of min heap

void minHeapify(Heap *h, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < h->size && h->arr[left] < h->arr[smallest])
        smallest = left;

    if (right < h->size && h->arr[right] < h->arr[smallest])
        smallest = right;

    if (smallest != i) {
        swap(&h->arr[i], &h->arr[smallest]);
        minHeapify(h, smallest);
    }
}

void insertMinHeap(Heap *h, int value) {
    h->arr[h->size] = value;
    int i = h->size;
    h->size++;

    while (i > 0 && h->arr[(i - 1) / 2] > h->arr[i]) {
        swap(&h->arr[i], &h->arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int extractMin(Heap *h) {
    if (h->size == 0) return -1;
    int root = h->arr[0];
    h->arr[0] = h->arr[h->size - 1];
    h->size--;
    minHeapify(h, 0);
    return root;
}
