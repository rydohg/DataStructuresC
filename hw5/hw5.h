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

void testSkipList(SkipList* list){
    QuadNode* bottomMostNode = list->head;
    for (int i = 0; i < list->maxHeight; ++i) {
        QuadNode* leftRightSearchNode = bottomMostNode;
        while (leftRightSearchNode->next != NULL){
            if(leftRightSearchNode->next->prev != leftRightSearchNode){
                printf("next/prev link broken\n");
            }
            leftRightSearchNode = leftRightSearchNode->next;
        }
        bottomMostNode = bottomMostNode->below;
    }
    int height = 0;
    QuadNode* leftRightSearchNode = bottomMostNode;
    while (leftRightSearchNode != NULL){
        //This doesn't account for when a above link is NULL when it shouldn't be
        QuadNode* upDownSearchNode = leftRightSearchNode;
        while (upDownSearchNode->above != NULL){
            if(upDownSearchNode->above->below != upDownSearchNode){
                printf("Above/below link broken. Height: %d Data: %d\n", height, upDownSearchNode->data->time);
            }
            height++;
            upDownSearchNode = upDownSearchNode->above;
        }
        height = 0;
        leftRightSearchNode = leftRightSearchNode->next;
    }
}

void testSkipFind(SkipList* list, int length){
    if(list->head == NULL){
        return;
    }
    for (int i = 0; i < length; ++i) {
        QuadNode* findNode = getActivity(list->head, i);
        if(findNode->data->time != i || findNode->above != NULL){
            printf("Didn't find right node");
        }
    }
}

/*    time_t time_struct;
    srand((unsigned) time(&time_struct));
*/
/*    const int TEST_LIST_SIZE = 5;
    for (int j = 0; j < 50; ++j) {
        for (int i = 0; i < TEST_LIST_SIZE; ++i) {
            addActivity(&list, createActivity("Test", i));
        }
        testSkipList(&list);
        testSkipFind(&list, TEST_LIST_SIZE);
        for (int i = 0; i < TEST_LIST_SIZE; ++i) {
            removeActivity(&list, i);
        }
        testSkipList(&list);
        testSkipFind(&list, TEST_LIST_SIZE);
    }*/
#endif //HW5_HW5_H
