#ifndef HW4_TEST_H
#define HW4_TEST_H

#endif //HW4_TEST_H

typedef struct {
    char* buyer;
    int time;
    double price;
    int quantity;
} Order;

int enterBuyOrder(char *buyer, int time, double price, int quantity);
int enterSellOrder(char *buyer, int time, double price, int quantity, Order** heap);
void displayHighestBuyOrder(int time);
void displayLowestSellOrder(int time, Order** sellerHeap);

int parent(int index) {
    return (index - 1)/2;
}
int left(int index) {
    return 2*index + 1;
}
int right(int index) {
    return 2*index + 2;
}
void swap(Order *a, Order *b) {
    Order temp = *a;
    *a = *b;
    *b = temp;
}
double removeMin(Order** sellerHeap);
void minHeapify(int index, Order** sellerHeap);
double removeMax(int index);
void maxHeapify(int index);
//void swap(double* a, double *b);
void insert(double a);
void delete(int index, Order** sellerHeap);
void printHeap();

void readInput(char* filename, Order** sellerHeap);