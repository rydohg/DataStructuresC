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
    double price;
    int priority;
} Item;

int enterBuyOrder(int time, char *buyer, double price, int quantity);
int enterSellOrder(int time, char *buyer, double price, int quantity);
void displayHighestBuyOrder(int time);
void displayLowestSellOrder(int time);

int parent(int index);
int left(int index);
int right(int index);
int removeMin(int index);
void heapify(int index);

Item* heap = NULL;

int main(int argc, char** args){
    if (argc < 3) {
        printf("Missing an input file");
        exit(1);
    }
    FILE *inputDataFile = fopen(args[1], "r");

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
                    enterBuyOrder(time, buyer, price, quantity);
                } else if(firstToken[5] == 'S'){
                    enterSellOrder(time, buyer, price, quantity);
                }

            } else if (firstToken[0] == 'D'){
                int time = atoi(token);
                if(firstToken[7] == 'H'){
                    displayHighestBuyOrder(time);
                } else if(firstToken[7] == 'L'){
                    displayLowestSellOrder(time);
                }
            }

            token = strtok(NULL, " ");
        }
    }
    fclose(inputDataFile);
    free(currentLine);
    return 0;
}

int enterBuyOrder(int time, char *buyer, double price, int quantity) {

}

int enterSellOrder(int time, char *buyer, double price, int quantity) {

}

void displayHighestBuyOrder(int time) {

}

void displayLowestSellOrder(int time) {

}
