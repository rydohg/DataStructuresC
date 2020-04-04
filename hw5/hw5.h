#ifndef HW5_HW5_H
#define HW5_HW5_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <zconf.h>

#include "fakeRandHeight.h"

// Data struct
typedef struct {
    char* activityName;
    int time;
} Activity;

// Skip Node
typedef struct quadNode {
    Activity* data;
    struct quadNode *prev, *next, *above, *below;
} QuadNode;

// A struct for keeping track of a skip list's head and height
typedef struct {
    QuadNode   *head;
    int maxHeight;
} SkipList;

void readInput(char* filename);
Activity* createActivity(char*, int time);
//SkipList functions
QuadNode *createSkipNode(Activity* data, QuadNode *prev, QuadNode *next, QuadNode *above, QuadNode *below);
void skipListInsert(SkipList* list, Activity* activity);
void printSkipList(SkipList* list);
#endif //HW5_HW5_H
