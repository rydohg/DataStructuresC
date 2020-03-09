#ifndef HW4_TEST_H
#define HW4_TEST_H

#endif //HW4_TEST_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char* buyer;
    int time;
    double price;
    int quantity;
} Order;

int enterBuyOrder(char *buyer, int time, double price, int quantity, Order** buyerHeap, Order** sellerHeap);
int enterSellOrder(char *buyer, int time, double price, int quantity, Order** sellerHeap, Order** buyerHeap);
void displayHighestBuyOrder(int time, Order** buyerHeap);
void displayLowestSellOrder(int time, Order** sellerHeap);
void execute(Order **buyerHeap, Order **sellerHeap);

int parent(int index) { return (index - 1)/2; }
int left(int index) { return 2*index + 1; }
int right(int index) { return 2*index + 2; }
void swap(Order *a, Order *b) {
    Order temp = *a;
    *a = *b;
    *b = temp;
}
int findPrice(double price, Order** heap, int heapSize);

double removeMin(Order** sellerHeap);
void minHeapify(int index, Order** sellerHeap);
double removeMax(Order** buyerHeap);
void maxHeapify(int index, Order** buyerHeap);
/*void insert(double a);
void delete(int index, Order** sellerHeap);*/
void printHeap(Order** heap, int heapSize);

void readInput(char* filename, Order** sellerHeap, Order** buyerHeap);

/*void insert(double a) {
    Order* idk = malloc(sizeof(Order));
    idk->price = a;
    idk->buyer = NULL;
    idk->quantity = 0;
    idk->time = 0;

    heap[sellerHeapSize] = idk;
    int index = sellerHeapSize;

    while (index != 0 && heap[parent(index)]->price > heap[index]->price){
        swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }
    sellerHeapSize++;
}*/

/*void delete(int index, Order** sellerHeap) {
    sellerHeap[index]->price = INT_MIN;

    while (index != 0 && heap[parent(index)]->price > heap[index]->price){
        swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }

    removeMin(sellerHeap);
}*/