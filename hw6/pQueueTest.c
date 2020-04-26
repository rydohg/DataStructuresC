/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:

*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    int number;
} Item;

typedef struct {
    Item** heap;
    int size;
    int emptyIndex;
} PriorityQueue;

// Helpers
int parent(int index) { return (index - 1)/2; }
int left(int index) { return 2*index + 1; }
int right(int index) { return 2*index + 2; }
void swap(Item *a, Item *b) {
    Item temp = *a;
    *a = *b;
    *b = temp;
}

void addToPQueue(int data, PriorityQueue* queue);
int removeMin(PriorityQueue* pQueue);
void minHeapify(int index, PriorityQueue* pQueue);

int main(int argc, char** args){
    PriorityQueue queue;
    queue.size = 20;

    addToPQueue(3, &queue);
    addToPQueue(4, &queue);
    addToPQueue(10, &queue);
    addToPQueue(1, &queue);
    addToPQueue(2, &queue);
    addToPQueue(8, &queue);
    addToPQueue(5, &queue);
    addToPQueue(7, &queue);
    addToPQueue(6, &queue);
    addToPQueue(11, &queue);
    addToPQueue(15, &queue);
    addToPQueue(12, &queue);
    addToPQueue(14, &queue);
    addToPQueue(13, &queue);

    /*Item* debugArray[queue.size];
    for (int j = 0; j < queue.size; ++j) {
        debugArray[j] = queue.heap[j];
    }*/

    for (int i = 0; i < queue.emptyIndex; ++i) {
        printf("%d ", queue.heap[i]->number);
    }
    printf("\n");
    while (queue.heap[0] != NULL){
        int min = removeMin(&queue);
        printf("%d ", min);
    }

    /*for (int j = 0; j < queue.size; ++j) {
        debugArray[j] = queue.heap[j];
    }*/
    return 0;
}

void addToPQueue(int data, PriorityQueue* queue){
    Item* newItem = malloc(sizeof(Item*));
    newItem->number = data;
    if(queue->heap == NULL){
        queue->heap = malloc(queue->size * sizeof(Item*));
        queue->heap[0] = newItem;
        queue->emptyIndex = 1;
    } else {
        queue->heap[queue->emptyIndex] = newItem;

        int index = queue->emptyIndex;
        while (index > 0 && queue->heap[parent(index)]->number > queue->heap[index]->number){
            swap(queue->heap[index], queue->heap[parent(index)]);
            index = parent(index);
        }
        queue->emptyIndex++;
    }
}

void minHeapify(int index, PriorityQueue* pQueue){
    int smallest = index;
    if (left(index) < pQueue->emptyIndex && pQueue->heap[left(index)]->number < pQueue->heap[index]->number)
        smallest = left(index);
    if (right(index) < pQueue->emptyIndex && pQueue->heap[right(index)]->number < pQueue->heap[smallest]->number)
        smallest = right(index);
    if (smallest != index){
        swap(pQueue->heap[index], pQueue->heap[smallest]);
        minHeapify(smallest, pQueue);
    }
}

int removeMin(PriorityQueue* pQueue){
    int min = pQueue->heap[0]->number;
    free(pQueue->heap[0]);
    pQueue->heap[0] = pQueue->heap[pQueue->emptyIndex-1];
    pQueue->heap[pQueue->emptyIndex - 1] = NULL;
    pQueue->emptyIndex--;

    minHeapify(0, pQueue);
    return min;
}
