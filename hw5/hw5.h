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

// A struct for keeping track of a SkipList's head and height
typedef struct {
    QuadNode   *head;
    int maxHeight;
} SkipList;

void readInput(char* filename, SkipList* list);
Activity* createActivity(char*, int time);
QuadNode *createSkipNode(Activity* data, QuadNode *prev, QuadNode *next, QuadNode *above, QuadNode *below);
void addActivity(SkipList* list, Activity* activity);
void removeActivity(SkipList *list,  int key);
QuadNode* getActivity(QuadNode *list, int key);
QuadNode* getActivitiesBetweenTimes(SkipList *list, int key1, int key2);
QuadNode* getActivitiesForOneDay(SkipList *list, int time);
QuadNode* getActivitiesEarlier(SkipList* list, int time);
void printSkipList(SkipList* list);
#endif //HW5_HW5_H
