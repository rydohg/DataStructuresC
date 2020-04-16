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