#ifndef HW6_HW6_H
#define HW6_HW6_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Adjacency list. Each node has a linked list of adjacent nodes.
typedef struct adjNode {
    int adjNodeIndex;
    struct adjNode* next;
} AdjacentNode;

typedef struct node {
    char data;
    AdjacentNode* adjNodes;
} GraphNode;

typedef struct {
    GraphNode** graphArray;
    int rowLength, colHeight;
    int emptyIndex;
} Graph;

#endif //HW6_HW6_H
