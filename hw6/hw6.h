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

typedef struct node {
    char data;
    NodeList* adjNodes;
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
