#ifndef HW6_HW6_H
#define HW6_HW6_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Adjacency list. Each node has a linked list of adjacent nodes.
typedef struct nodeList {
    int nodeIndex;
    struct nodeList* next;
} NodeList;

/*typedef struct node {
    char data;
    NodeList* adjNodes;
    int label;
    int bfsParentIndex;
} GraphNode;*/

typedef struct node {
    char data;
    int adjNodes[4];
    int label;
    int bfsParentIndex;
} GraphNode;

typedef struct {
    GraphNode** graphArray;
    int rowLength, colHeight;
    int emptyIndex;
    int tronIndex;
    NodeList* bugs;
} Graph;

typedef struct {
    int* queue;
    int front, back, arraySize, queueSize;
} Queue;

#endif //HW6_HW6_H

/*
    NodeList* newNode = malloc(sizeof(NodeList));
    newNode->nodeIndex = nodeIndex;

    if(*list == NULL){
        *list = newNode;
        newNode->length = 1;
    } else {
        if((*list)->nodeIndex > nodeIndex){
            newNode->next = *list;
            newNode->length = (*list)->length + 1;
            *list = newNode;
        } else {
            // This puts the indices in order by sorting since up < left < right < down
            NodeList* prevNode = *list;
            NodeList* tempNode = (*list)->next;
            while (tempNode != NULL && tempNode->nodeIndex < nodeIndex){
                prevNode = tempNode;
                tempNode = tempNode->next;
            }
            newNode->next = tempNode;
            prevNode->next = newNode;
            (*list)->length++;
        }
    }
    */

/*printf("%c: (%d, %d)\n", neighborNode->data, neighbors[i] % graph->rowLength,
                           neighbors[i] / graph->colHeight);*/

/*
*/

/*
void printParents(Graph *maze);
void printParents(Graph *maze) {
    printf("--------------\n");
    for (int i = 0; i < maze->rowLength * maze->colHeight; ++i) {
        printf("%02d:%02d ", i, maze->graphArray[i]->bfsParentIndex);
        // Add a newline after the last char in  a row
        if (i % maze->rowLength == maze->rowLength - 1) {
            printf("\n");
        }
    }
    printf("--------------\n");
}*/

// Turn the graph's array into a traditional array so it can be represented in CLion's debugger
/*
GraphNode *debugArray[maze.rowLength * maze.colHeight];
int i;
for (i = 0; i < maze.rowLength * maze.colHeight; ++i) {
debugArray[i] = maze.graphArray[i];
}*/

/*int nextVertex = maze->tronIndex;
    while (nextVertex != -1){
        printf(" (%d,%d)", nextVertex / maze->rowLength, nextVertex % maze->rowLength);
        nextVertex = maze->graphArray[nextVertex]->bfsParentIndex;
    }*/
/*
void printDijkstra(Graph* maze) {
    int i;
    for (i = 0; i < maze->rowLength * maze->colHeight; ++i) {
        int label = maze->graphArray[i]->dijkstraLabel;
        if(label != INT_MAX){
            char labelString[4];
            snprintf(labelString, 3, "%d", label);
            if(strlen(labelString) == 1){
                printf("  ");
            } else if(strlen(labelString) == 2){
                printf(" ");
            }
            printf("%d", maze->graphArray[i]->dijkstraLabel);
        } else {
            printf("  M");
        }

        // Add a newline after the last char in  a row
        if (i % maze->rowLength == maze->rowLength - 1) {
            printf("\n");
        }
    }
}*/
