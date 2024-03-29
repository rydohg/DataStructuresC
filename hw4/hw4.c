/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:
  Creates and maintains min and max heaps for sellers and buyers respectively
  to best match sellers to buyers of our fake cryptocurrency
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char* name;
    int time;
    double price;
    int quantity;
} Order;

//Functions related to the commands this program can take
int enterBuyOrder(char *buyer, int time, double price, int quantity, Order** buyerHeap, Order** sellerHeap);
int enterSellOrder(char *seller, int time, double price, int quantity, Order** sellerHeap, Order** buyerHeap);
void displayHighestBuyOrder(int time, Order** buyerHeap);
void displayLowestSellOrder(int time, Order** sellerHeap);
void execute(Order **buyerHeap, Order **sellerHeap, double price);

//Heap functions
void minHeapify(int index, Order** sellerHeap);
void maxHeapify(int index, Order** buyerHeap);
double removeMin(Order** sellerHeap);
double removeMax(Order** buyerHeap);

//Helper functions
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

void readInput(char* filename, Order** sellerHeap, Order** buyerHeap);

int sellerHeapSize = 0;
int buyerHeapSize = 0;

int main(int argc, char** args){
    if (argc < 2) {
        printf("Missing an input file");
        exit(1);
    }
    int arrSize = 30;
    Order** sellerHeap = calloc(sizeof(Order), arrSize);
    Order** buyerHeap = calloc(sizeof(Order), arrSize);

    readInput(args[1], sellerHeap, buyerHeap);

    int i;
    for (i = 0; i < sellerHeapSize; ++i) {
        free(sellerHeap[i]->name);
        free(sellerHeap[i]);
    }
    for (i = 0; i < buyerHeapSize; ++i) {
        free(buyerHeap[i]->name);
        free(buyerHeap[i]);
    }
    free(sellerHeap);
    free(buyerHeap);
    return 0;
}

void readInput(char *filename, Order** sellerHeap, Order** buyerHeap) {
    FILE *inputDataFile = fopen(filename, "r");

    char *currentLine = malloc(128 * sizeof(char));
    while (fgets(currentLine, 128 - 1, inputDataFile) != NULL) {
        // First call to strtok returns the first token
        char *token = strtok(currentLine, " ");
        char *firstToken = malloc(strlen(token) + 1);
        strcpy(firstToken, token);

        firstToken[strcspn(firstToken, "\n")] = 0;

        // Move strtok to the next token
        token = strtok(NULL, " ");

        while (token != NULL) {
            //If we are entering an order
            if(firstToken[0] == 'E'){
                // The first token is the time, the second is the name, the third is price, and the fourth is the quantity
                int time = atoi(token);

                token = strtok(NULL, " ");
                char* name = malloc(strlen(token) + 1);
                strcpy(name, token);

                token = strtok(NULL, " ");
                double price = atof(token);

                token = strtok(NULL, " ");
                int quantity = atoi(token);

                //If index 5 is B then its EnterBuyOrder and if its S then its EnterSellOrder
                if(firstToken[5] == 'B'){
                    enterBuyOrder(name, time, price, quantity, buyerHeap, sellerHeap);
                } else if(firstToken[5] == 'S'){
                    enterSellOrder(name, time, price, quantity, sellerHeap, buyerHeap);
                }
            }
            //Otherwise its a display command
            else if (firstToken[0] == 'D'){
                int time = atoi(token);
                //DisplayHighest
                if(firstToken[7] == 'H'){
                    displayHighestBuyOrder(time, buyerHeap);
                }
                //DisplayLowest
                else if(firstToken[7] == 'L'){
                    displayLowestSellOrder(time, sellerHeap);
                }
            }

            token = strtok(NULL, " ");
        }
        free(firstToken);
    }
    fclose(inputDataFile);
    free(currentLine);
}

int enterBuyOrder(char *buyer, int time,  double price, int quantity, Order** buyerHeap, Order** sellerHeap) {
    Order* newOrder = malloc(sizeof(Order));
    newOrder->name = buyer;
    newOrder->time = time;
    newOrder->price = price;
    newOrder->quantity = quantity;

    //Print 6 digits for the timestamp and 2 decimal places for the price
    printf("EnterBuyOrder %06d %s %.2lf %d\n", time, buyer, price, quantity);

    //Add newOrder to heap and move it to the right place
    buyerHeap[buyerHeapSize] = newOrder;
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

    //Execute if there's a matching sell order or if there's a sell order cheaper than the buy order
    int sellerPriceIndex = findPrice(price, sellerHeap, sellerHeapSize);
    if(sellerHeap[0] != NULL && sellerPriceIndex != 1){
        if(price > sellerHeap[0]->price){
            execute(buyerHeap, sellerHeap, (price + sellerHeap[0]->price)/2);
        } else if (price == sellerHeap[0]->price){
            execute(buyerHeap, sellerHeap, price);
        }
    }
    return 0;
}

void execute(Order **buyerHeap, Order **sellerHeap, double price) {
    int quantity;
    //Keep track if the buyer wants more than the seller is offering so we can subtract the values accordingly
    int buyerWantsMore = 0;
    if(buyerHeap[0]->quantity > sellerHeap[0]->quantity){
        quantity = sellerHeap[0]->quantity;
        buyerWantsMore = 1;
    } else {
        quantity = buyerHeap[0]->quantity;
    }

    printf("ExecuteBuySellOrders %.2lf %d\n", price, quantity);
    if(buyerWantsMore){
        buyerHeap[0]->quantity -= quantity;
        sellerHeap[0]->quantity = 0;
    } else {
        sellerHeap[0]->quantity -= quantity;
        buyerHeap[0]->quantity = 0;
    }

    printf("Buyer: %s %d\nSeller: %s %d\n", buyerHeap[0]->name, buyerHeap[0]->quantity, sellerHeap[0]->name, sellerHeap[0]->quantity);
    //Remove from the heaps if there's no quantity left in an order
    if(sellerHeap[0]->quantity <= 0){
        removeMin(sellerHeap);
    }
    if(buyerHeap[0]->quantity <= 0) {
        removeMax(buyerHeap);
    }
}

int enterSellOrder(char *seller, int time,  double price, int quantity, Order** sellerHeap, Order** buyerHeap) {
    Order* newOrder = malloc(sizeof(Order));
    newOrder->name = seller;
    newOrder->time = time;
    newOrder->price = price;
    newOrder->quantity = quantity;

    //Print 6 digits for the timestamp and 2 decimal places for the price
    printf("EnterSellOrder %06d %s %.2lf %d\n", time, seller, price, quantity);
    //Add newOrder to heap and move it to the right place
    sellerHeap[sellerHeapSize] = newOrder;
    int index = sellerHeapSize;

    while (index > 0 && sellerHeap[parent(index)]->price >= sellerHeap[index]->price){
        // If there's a price tie then only swap if the new seller is earlier
        if(sellerHeap[parent(index)]->price == sellerHeap[index]->price && sellerHeap[parent(index)]->time < sellerHeap[index]->time){
            break;
        }

        swap(sellerHeap[index], sellerHeap[parent(index)]);
        index = parent(index);
    }
    sellerHeapSize++;

    //Execute if there's a matching buy order or if there's a buy order offering more money than the sell price
    if(buyerHeap[0] != NULL && price < buyerHeap[0]->price){
        execute(buyerHeap, sellerHeap, (price + buyerHeap[0]->price)/2);
    } else if(buyerHeap[0] != NULL && price == buyerHeap[0]->price){
        execute(buyerHeap, sellerHeap, price);
    }
    return 0;
}

void displayHighestBuyOrder(int time, Order** buyerHeap) {
    printf("DisplayHighestBuyOrder %06d ", time);
    if(buyerHeap[0]->time < time && buyerHeap[0]->name != NULL){
        printf("%s %06d %.2lf %d", buyerHeap[0]->name, buyerHeap[0]->time, buyerHeap[0]->price, buyerHeap[0]->quantity);
    }
    printf("\n");
}

void displayLowestSellOrder(int time, Order** sellerHeap) {
    printf("DisplayLowestSellOrder %06d", time);
    if(sellerHeap[0]->time < time && sellerHeap[0]->name != NULL){
        printf(" %s %06d %.2lf %d", sellerHeap[0]->name, sellerHeap[0]->time, sellerHeap[0]->price, sellerHeap[0]->quantity);
    }
    printf("\n");
}

void printHeap(Order** heap, int heapSize){
    int i;
    for (i = 0; i < heapSize; ++i) {
        printf("%s %.2lf %d %d\n", heap[i]->name, heap[i]->price, heap[i]->quantity, heap[i]->time);
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
    free(buyerHeap[0]);
    buyerHeap[0] = buyerHeap[buyerHeapSize-1];
    buyerHeapSize--;
    maxHeapify(0, buyerHeap);

    return max;
}

int findPrice(double price, Order** heap, int heapSize){
    int i;
    for (i = 0; i < heapSize; ++i) {
        if(price == heap[i]->price){
            return i;
        }
    }
    return -1;
}