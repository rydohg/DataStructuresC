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
#include <zconf.h>
#include "test.h"

Order** heap;
int sellerHeapSize = 0;

int main(int argc, char** args){
    if (argc < 2) {
        printf("Missing an input file");
        exit(1);
    }
    int arrSize = 15;
    Order** sellerHeap = calloc(sizeof(Order), arrSize);
/*    enterSellOrder("idk", 12345, 3, 1, sellerHeap);
    enterSellOrder("idk", 12345, 2, 1, sellerHeap);
    enterSellOrder("idk", 12345, 15, 1, sellerHeap);
    enterSellOrder("idk", 12345, 5, 1, sellerHeap);
    enterSellOrder("idk", 12345, 4, 1, sellerHeap);
    enterSellOrder("idk", 12345, 45, 1, sellerHeap);*/
//    insert(2);
//    delete(1);
//    insert(15);
//    insert(5);
//    insert(4);
//    insert(45);
    readInput(args[1], sellerHeap);
/*    for (int i = 0; i < sellerHeapSize; ++i) {
        printf("%s %d %lf\n", sellerHeap[i]->buyer, sellerHeap[i]->time, sellerHeap[i]->price);
    }*/

    for (int i = 0; i < sellerHeapSize; ++i) {
        free(sellerHeap[i]);
    }
    return 0;
}

void readInput(char *filename, Order** sellerHeap) {
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
                    enterBuyOrder(buyer, time, price, quantity);
                } else if(firstToken[5] == 'S'){
                    enterSellOrder(buyer, time, price, quantity, sellerHeap);
                }

            } else if (firstToken[0] == 'D'){
                int time = atoi(token);
                if(firstToken[7] == 'H'){
                    displayHighestBuyOrder(time);
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

int enterBuyOrder(char *buyer, int time,  double price, int quantity) {

}

int enterSellOrder(char *buyer, int time,  double price, int quantity, Order** sellerHeap) {
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
    return 0;
}

void displayHighestBuyOrder(int time) {

}

void displayLowestSellOrder(int time, Order** sellerHeap) {
    if(sellerHeap[0]->time < time){
        printf("DisplayLowestSeller: %s %.2lf %d\n", sellerHeap[0]->buyer, sellerHeap[0]->price, sellerHeap[0]->quantity);
    }
}

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

void delete(int index, Order** sellerHeap) {
    sellerHeap[index]->price = INT_MIN;

    while (index != 0 && heap[parent(index)]->price > heap[index]->price){
        swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }

    removeMin(sellerHeap);
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