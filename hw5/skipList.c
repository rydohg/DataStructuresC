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
    QuadNode *minNode = createSkipNode(INT_MIN, NULL, NULL, NULL, NULL);
    QuadNode *maxNode = createSkipNode(INT_MAX, NULL, NULL, NULL, NULL);

    // empty list
    if (list->head == NULL) {
        new_node = createSkipNode(key, minNode, maxNode, NULL, NULL);
        list->head = new_node;

        QuadNode *prev_node = list->head;
        for (int i = 0; i < height; ++i) {
            QuadNode *node = createSkipNode(key, minNode, maxNode, prev_node, NULL);
            node->above = prev_node;
            prev_node->below = node;

            prev_node = node;
        }
        list->maxHeight = height;
    }
    // Change the head if the height of the new node is greater than the maxHeight in the current skip list
    else if (height > list->maxHeight) {
        QuadNode *oldHead = list->head;

        // Create a new list at the top of the skip list between -inf and +inf
        new_node = createSkipNode(key, minNode, maxNode, NULL, NULL);
        list->head = new_node;
        //Add its below nodes and TODO:sort them in the below lists
        QuadNode *prev_node = new_node;
        QuadNode* level = NULL;
        for (int i = height - 1; i >= 0; --i) {
            QuadNode* node;
            if(list->maxHeight < i){
                node = createSkipNode(key, minNode, maxNode, prev_node, NULL);
                prev_node->below = node;
            } else if(list->maxHeight == i){
                if(oldHead->data > key){
                    node = createSkipNode(key, minNode, oldHead, prev_node, NULL);
                    prev_node->below = node;
                    level = oldHead->below;
                } else {
                    // Search through this level to find where this key goes
                    QuadNode* searchNode = oldHead;
                    while (searchNode->next->data < key){
                        searchNode = searchNode->next;
                    }
                    node = createSkipNode(key, searchNode, searchNode->next, prev_node, NULL);
                    searchNode->next->prev = node;
                    searchNode->next = node;

                    prev_node->below = node;
                    level = searchNode->below;
                }
            } else if (list->maxHeight > i){
                if(level != NULL) {
                    QuadNode* searchNode = level;
                    if (level->data < key) {
                        while (searchNode->next->data < key){
                            searchNode = searchNode->next;
                        }
                    } else {
                        while (searchNode->next->data < key){
                            searchNode = searchNode->prev;
                        }
                    }
                    node = createSkipNode(key, searchNode, searchNode->next, prev_node, NULL);
                    searchNode->next->prev = node;
                    searchNode->next = node;
                    prev_node->below = node;
                    level = level->below;
                }
            }
            prev_node = node;
        }
        list->maxHeight = height;
    } else if (height == list->maxHeight){
        QuadNode* topmostNode;
        if(key < list->head->data){
            topmostNode = createSkipNode(key, minNode, list->head, NULL, NULL);
            list->head->prev = topmostNode;
            list->head = topmostNode;
        } else {
            QuadNode* searchNode = list->head;
            while (searchNode->next->data < key){
                searchNode = searchNode->next;
            }
            topmostNode = createSkipNode(key, searchNode, searchNode->next, NULL, NULL);
            searchNode->next->prev = topmostNode;
            searchNode->next = topmostNode;
        }

        QuadNode* prev_node = topmostNode;
        QuadNode* level = list->head->below;
        for (int i = height - 1; i >= 0; --i) {
            QuadNode* node;
            if (list->maxHeight > i){
                if(level != NULL) {
                    QuadNode* searchNode = level;
                    if (level->data < key) {
                        while (searchNode->next->data < key){
                            searchNode = searchNode->next;
                        }
                    } else {
                        while (searchNode->next->data < key){
                            searchNode = searchNode->prev;
                        }
                    }
                    node = createSkipNode(key, searchNode, searchNode->next, prev_node, NULL);
                    searchNode->next->prev = node;
                    searchNode->next = node;
                    prev_node->below = node;
                    level = level->below;
                }
            }
            prev_node = node;
        }
    }
}

void printSkipList(SkipList *list) {
    QuadNode *tempNode = list->head; // current node

    int counter = list->maxHeight;
    while (tempNode != NULL){
        printf("Level %d: ", counter);
        QuadNode* levelTempNode = tempNode;

        // Move levelTempNode all the way to the left
        while (levelTempNode->prev->data != INT_MIN){
            levelTempNode = levelTempNode->prev;
        }
        while (levelTempNode->next != NULL){
            printf("%d ", levelTempNode->data);
            levelTempNode = levelTempNode->next;
        }
        printf("\n");
        counter--;
        tempNode = tempNode->below;
    }
}

// uncomment #define DEBUG near the top
#ifdef DEBUG

int main(int argc, char *argv[]) {
    SkipList list;
    list.head = NULL;
    list.maxHeight = 0;

    skipListInsert(&list, 1);
    skipListInsert(&list, 2);
    skipListInsert(&list, 3);
    skipListInsert(&list, 4);
    skipListInsert(&list, 5);
    skipListInsert(&list, 6);
    printSkipList(&list);
}
#endif