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
void execute(Order **buyerHeap, Order **sellerHeap, double d);

void minHeapify(int index, Order** sellerHeap);
void maxHeapify(int index, Order** buyerHeap);
double removeMin(Order** sellerHeap);
double removeMax(Order** buyerHeap);

void readInput(char* filename, Order** sellerHeap, Order** buyerHeap);
int parent(int index) { return (index - 1)/2; }
int left(int index) { return 2*index + 1; }
int right(int index) { return 2*index + 2; }
void swap(Order *a, Order *b) {
    Order temp = *a;
    *a = *b;
    *b = temp;
}
int findPrice(double price, Order** heap, int heapSize);
void printHeap(Order** heap, int heapSize);

/*
    enterSellOrder("s a", 1000, 3, 1, sellerHeap, buyerHeap);
    enterBuyOrder("b b", 2000, 3, 1, buyerHeap, sellerHeap);
    enterSellOrder("s c", 3000, 15, 2, sellerHeap, buyerHeap);
    enterBuyOrder("b d", 4000, 15, 1, buyerHeap, sellerHeap);
*/