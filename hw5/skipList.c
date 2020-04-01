/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Description:
  Implementation of a skip list
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>

#include "skipList.h"
#include "fakeRandHeight.h"

#define DEBUG

// create and return a node with data and previous/next nodes
QuadNode *createSkipNode(int data, QuadNode *prev, QuadNode *next, QuadNode *above, QuadNode *below) {
    QuadNode *newNode = (QuadNode *) malloc(sizeof(QuadNode));
    if (newNode == NULL) {
        printf("malloc failed");
        exit(-1);
    }

    newNode->data = data;
    newNode->prev = prev;
    newNode->next = next;
    newNode->above = above;
    newNode->below = below;

    return newNode;
}

void skipListInsert(SkipList *list, int key) {
    QuadNode *new_node = NULL;   // node to be inserted

    int height = getRandHeight();

    // empty list
    if (list->head == NULL) {
        QuadNode *minNode = createSkipNode(INT_MIN, NULL, NULL, NULL, NULL);
        QuadNode *maxNode = createSkipNode(INT_MAX, minNode, NULL, NULL, NULL);
        new_node = createSkipNode(key, minNode, maxNode, NULL, NULL);

        minNode->next = new_node;
        maxNode->prev = new_node;

        list->head = minNode;

        QuadNode *prev_node = list->head;
        for (int i = 0; i < height; ++i) {
            //TODO: rename
            QuadNode *newMinNode = createSkipNode(INT_MIN, NULL, NULL, prev_node, NULL);
            QuadNode *node = createSkipNode(key, newMinNode, NULL, prev_node->next, NULL);
            QuadNode *newMaxNode = createSkipNode(INT_MAX, NULL, NULL, prev_node->next->next, NULL);
            node->next = newMaxNode;
            newMinNode->next = node;

            prev_node->below = newMinNode;
            prev_node->next->below = node;
            prev_node->next->next->below = newMaxNode;

            prev_node = newMinNode;
        }
        list->maxHeight = height;
    }
    else if (height > list->maxHeight) {
        //TODO: Optimize this section
        QuadNode* oldHead = list->head;

        // Copied the code for making the initial skip list so all we need to do is connect it
        QuadNode *topMinNode = createSkipNode(INT_MIN, NULL, NULL, NULL, NULL);
        QuadNode *topMaxNode = createSkipNode(INT_MAX, topMinNode, NULL, NULL, NULL);
        QuadNode *topNewNode = createSkipNode(key, topMinNode, topMaxNode, NULL, NULL);

        topMinNode->next = topNewNode;
        topMaxNode->prev = topNewNode;

        list->head = topMinNode;

        QuadNode *aboveMinNode = list->head;
        QuadNode *aboveMaxNode = list->head->next->next;
        QuadNode *node = topNewNode;
        for (int i = 0; i < height - list->maxHeight - 1; ++i) {
            QuadNode *newMinNode = createSkipNode(INT_MIN, NULL, NULL, aboveMinNode, NULL);
            node = createSkipNode(key, newMinNode, NULL, aboveMinNode->next, NULL);
            QuadNode* newMaxNode = createSkipNode(INT_MAX, NULL, NULL, aboveMinNode->next->next, NULL);
            node->next = newMaxNode;
            newMinNode->next = node;

            aboveMinNode->below = newMinNode;
            aboveMinNode->next->below = node;
            aboveMinNode->next->next->below = newMaxNode;

            aboveMinNode = newMinNode;
            aboveMaxNode = newMaxNode;
        }

        // Here we have the 3 above nodes and we're going to link them to the old SkipList
        aboveMinNode->below = oldHead;
        QuadNode* searchNode = oldHead;
        while (key > searchNode->next->data){
            searchNode = searchNode->next;
        }

        // Once we find where this goes lets put it there
        QuadNode* newNode = createSkipNode(key, searchNode, searchNode->next, node, NULL);
        searchNode->next->prev = newNode;
        searchNode->next = newNode;
        node->below = newNode;

        // Adjust the pointers on the min and max nodes
        aboveMinNode->below = oldHead;
        QuadNode* tempNode = searchNode;
        while (tempNode->next != NULL){
            tempNode = tempNode->next;
        }
        aboveMaxNode->below = tempNode;

        // Add and link up the new node in the rest of the lists below
        // We start looking below the node we just inserted after
        QuadNode* aboveNode;
        QuadNode* currentLevel = searchNode->below;
        for (int j = list->maxHeight - 1; j >= 0; --j) {
            while (key > currentLevel->next->data){
                currentLevel = currentLevel->next;
            }
            aboveNode = newNode;
            newNode = createSkipNode(key, currentLevel, currentLevel->next, aboveNode, NULL);
            currentLevel->next->prev = newNode;
            currentLevel->next = newNode;
            aboveNode->below = newNode;

            //Go down now
            currentLevel = currentLevel->below;
        }
        list->maxHeight = height;
    } else if (height == list->maxHeight){
        QuadNode* searchNode = list->head;
        while (key > searchNode->next->data){
            searchNode = searchNode->next;
        }
        QuadNode* newNode = createSkipNode(key, searchNode, searchNode->next,NULL, NULL);
        searchNode->next->prev = newNode;
        searchNode->next = newNode;

        QuadNode* aboveNode = newNode;
        QuadNode* currentLevel = searchNode->below;
        for (int j = list->maxHeight - 1; j >= 0; --j) {
            while (key > currentLevel->next->data){
                currentLevel = currentLevel->next;
            }
            newNode = createSkipNode(key, currentLevel, currentLevel->next, aboveNode, NULL);
            currentLevel->next->prev = newNode;
            currentLevel->next = newNode;
            aboveNode->below = newNode;

            //Go down now
            aboveNode = newNode;
            currentLevel = currentLevel->below;
        }
    } else {
        QuadNode* aboveNode = NULL;
        QuadNode* currentLevel = list->head;
        for (int i = 0; i < list->maxHeight - height; ++i) {
            currentLevel = currentLevel->below;
        }
        for (int j = height; j >= 0; --j) {
            while (key > currentLevel->next->data){
                currentLevel = currentLevel->next;
            }
            QuadNode* newNode = createSkipNode(key, currentLevel, currentLevel->next, aboveNode, NULL);
            currentLevel->next->prev = newNode;
            currentLevel->next = newNode;
            if(aboveNode != NULL){
                aboveNode->below = newNode;
            }

            //Go down now
            aboveNode = newNode;
            currentLevel = currentLevel->below;
        }
    }
}

void printSkipList(SkipList *list) {
    QuadNode *tempNode = list->head; // current node

    int counter = list->maxHeight;
    while (tempNode != NULL){
        printf("Level %d: ", counter);
        QuadNode* levelTempNode = tempNode;
        while (levelTempNode->next != NULL){
            if(levelTempNode->data != INT_MIN && levelTempNode->data != INT_MAX){
                printf("%d ", levelTempNode->data);
            }
            levelTempNode = levelTempNode->next;
        }
        printf("\n");
        counter--;
        tempNode = tempNode->below;
    }
}

// uncomment #define DEBUG near the top
#ifdef DEBUG
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
                printf("Above/below link broken\n");
            }
            height++;
            upDownSearchNode = upDownSearchNode->above;
        }
        height = 0;
        leftRightSearchNode = leftRightSearchNode->next;
    }
}

int main(int argc, char *argv[]) {
    SkipList list;
    list.head = NULL;
    list.maxHeight = 0;

    for (int i = 0; i < 7; ++i) {
        skipListInsert(&list, i);
    }
    printSkipList(&list);
    testSkipList(&list);

    printf("List 2\n");
    SkipList list2;
    list2.head = NULL;
    list2.maxHeight = 0;

    for (int j = 7; j >= 0; --j) {
        skipListInsert(&list2, j);
    }
    printSkipList(&list2);
    testSkipList(&list2);
}
#endif