/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:
*/

#include <zconf.h>
#include "test.h"

int sellerHeapSize = 0;
int buyerHeapSize = 0;

int main(int argc, char** args){
    if (argc < 2) {
        printf("Missing an input file");
        exit(1);
    }
    int arrSize = 15;
    Order** sellerHeap = calloc(sizeof(Order), arrSize);
    Order** buyerHeap = calloc(sizeof(Order), arrSize);

    enterSellOrder("s a", 1000, 3, 1, sellerHeap, buyerHeap);
    enterBuyOrder("b b", 2000, 3, 1, buyerHeap, sellerHeap);
    enterSellOrder("s c", 3000, 15, 2, sellerHeap, buyerHeap);
    enterBuyOrder("b d", 4000, 15, 1, buyerHeap, sellerHeap);

//    readInput(args[1], sellerHeap, buyerHeap);

    for (int i = 0; i < sellerHeapSize; ++i) {
        free(sellerHeap[i]);
    }
    return 0;
}

void readInput(char *filename, Order** sellerHeap, Order** buyerHeap) {
    FILE *inputDataFile = fopen(filename, "r");

    char *currentLine = malloc(128 * sizeof(char));
    while (fgets(currentLine, 128 - 1, inputDataFile) != NULL) {
        // First call to strtok returns the first token
        char *token = strtok(currentLine, " ");
        char *firstToken = malloc(strlen(token));
        strcpy(firstToken, token);

        firstToken[strcspn(firstToken, "\n")] = 0;

        // Move strtok to the next token
        token = strtok(NULL, " ");

        while (token != NULL) {
            if(firstToken[0] == 'E'){
                int time = atoi(token);

                token = strtok(NULL, " ");
                char* buyer = malloc(strlen(token));
                strcpy(buyer, token);

                token = strtok(NULL, " ");
                double price = atof(token);

                token = strtok(NULL, " ");
                int quantity = atoi(token);

                if(firstToken[5] == 'B'){
                    enterBuyOrder(buyer, time, price, quantity, buyerHeap, sellerHeap);
                } else if(firstToken[5] == 'S'){
                    enterSellOrder(buyer, time, price, quantity, sellerHeap, buyerHeap);
                }

            } else if (firstToken[0] == 'D'){
                int time = atoi(token);
                if(firstToken[7] == 'H'){
                    displayHighestBuyOrder(time, buyerHeap);
                } else if(firstToken[7] == 'L'){
                    displayLowestSellOrder(time, sellerHeap);
                }
            }

            token = strtok(NULL, " ");
        }
    }
    fclose(inputDataFile);
    free(currentLine);
}

int enterBuyOrder(char *buyer, int time,  double price, int quantity, Order** buyerHeap, Order** sellerHeap) {
    Order* idk = malloc(sizeof(Order));
    idk->buyer = buyer;
    idk->time = time;
    idk->price = price;
    idk->quantity = quantity;

    printf("EnterBuyOrder: %s %.2lf %d\n", buyer, price, quantity);
    buyerHeap[buyerHeapSize] = idk;
    int index = buyerHeapSize;

    while (index != 0 && buyerHeap[parent(index)]->price <= buyerHeap[index]->price){
        // If there's a price tie then only swap if the new seller is earlier
        if(buyerHeap[parent(index)]->price == buyerHeap[index]->price && buyerHeap[parent(index)]->time < buyerHeap[index]->time){
            break;
        }

        swap(buyerHeap[index], buyerHeap[parent(index)]);
        index = parent(index);
    }
    buyerHeapSize++;

    int sellerPriceIndex = findPrice(price, sellerHeap, sellerHeapSize);
    if(sellerHeap[0] != NULL && sellerPriceIndex != 1){
        execute(buyerHeap, sellerHeap);
    }
    return 0;
}

void execute(Order **buyerHeap, Order **sellerHeap) {
    /*if(buyerHeap[0]->price > sellerHeap[0]->price){}*/
    printf("Before:\nSellers:\n");
    printHeap(sellerHeap, sellerHeapSize);
    printf("Buyers:\n");
    printHeap(buyerHeap, buyerHeapSize);

    int buyerOrderSize = buyerHeap[0]->quantity;
    buyerHeap[0]->quantity -= sellerHeap[0]->quantity;
    sellerHeap[0]->quantity -= buyerOrderSize;
    if(sellerHeap[0]->quantity <= 0){
        removeMin(sellerHeap);
    }
    if(buyerHeap[0]->quantity <= 0) {
        removeMax(buyerHeap);
    }

    printf("After:\nSellers:\n");
    printHeap(sellerHeap, sellerHeapSize);
    printf("Buyers:\n");
    printHeap(buyerHeap, buyerHeapSize);
}

int enterSellOrder(char *buyer, int time,  double price, int quantity, Order** sellerHeap, Order** buyerHeap) {
    Order* idk = malloc(sizeof(Order));
    idk->buyer = buyer;
    idk->time = time;
    idk->price = price;
    idk->quantity = quantity;

    printf("EnterSellOrder: %s %.2lf %d\n", buyer, price, quantity);
    sellerHeap[sellerHeapSize] = idk;
    int index = sellerHeapSize;

    while (index != 0 && sellerHeap[parent(index)]->price >= sellerHeap[index]->price){
        // If there's a price tie then only swap if the new seller is earlier
        if(sellerHeap[parent(index)]->price == sellerHeap[index]->price && sellerHeap[parent(index)]->time < sellerHeap[index]->time){
            break;
        }

        swap(sellerHeap[index], sellerHeap[parent(index)]);
        index = parent(index);
    }
    sellerHeapSize++;

    if(buyerHeap[0] != NULL && price == buyerHeap[0]->price){
        execute(buyerHeap, sellerHeap);
    }
    return 0;
}

void displayHighestBuyOrder(int time, Order** buyerHeap) {
    if(buyerHeap[0]->time < time){
        printf("DisplayHighestBuyer: %s %.2lf %d\n", buyerHeap[0]->buyer, buyerHeap[0]->price, buyerHeap[0]->quantity);
    }
}

void displayLowestSellOrder(int time, Order** sellerHeap) {
    if(sellerHeap[0]->time < time){
        printf("DisplayLowestSeller: %s %.2lf %d\n", sellerHeap[0]->buyer, sellerHeap[0]->price, sellerHeap[0]->quantity);
    }
}

void printHeap(Order** heap, int heapSize){
    for (int i = 0; i < heapSize; ++i) {
        printf("%s %.2lf %d %d\n", heap[i]->buyer, heap[i]->price, heap[i]->quantity, heap[i]->time);
    }
}

void minHeapify(int index, Order** sellerHeap){
    int smallest = index;
    if (left(index) < sellerHeapSize && sellerHeap[left(index)]->price < sellerHeap[index]->price)
        smallest = left(index);
    if (right(index) < sellerHeapSize && sellerHeap[right(index)]->price < sellerHeap[smallest]->price)
        smallest = right(index);
    if (smallest != index){
        swap(sellerHeap[index], sellerHeap[smallest]);
        minHeapify(smallest, sellerHeap);
    }
}

double removeMin(Order** sellerHeap){
    double min = sellerHeap[0]->price;
    free(sellerHeap[0]);
    sellerHeap[0] = sellerHeap[sellerHeapSize-1];
    sellerHeapSize--;

    minHeapify(0, sellerHeap);

    return min;
}

void maxHeapify(int index, Order** buyerHeap){
    int max = index;
    if (left(index) < buyerHeapSize && buyerHeap[left(index)]->price > buyerHeap[index]->price)
        max = left(index);
    if (right(index) < buyerHeapSize && buyerHeap[right(index)]->price > buyerHeap[max]->price)
        max = right(index);
    if (max != index){
        swap(buyerHeap[index], buyerHeap[max]);
        maxHeapify(max, buyerHeap);
    }
}

double removeMax(Order** buyerHeap){
    double max = buyerHeap[0]->price;
    //Send it to the ascendant realm
    buyerHeap[0]->price = INT_MIN;
    maxHeapify(0, buyerHeap);

    buyerHeapSize--;
    return max;
}

int findPrice(double price, Order** heap, int heapSize){
    for (int i = 0; i < heapSize; ++i) {
        if(price == heap[i]->price){
            return i;
        }
    }
    return -1;
}