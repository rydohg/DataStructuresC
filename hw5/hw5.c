/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:
  This homework implements a SkipList whose nodes hold activities which have a name and a time.
  It takes commands from a given file which performs operations on the SkipList.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
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
QuadNode* getActivitiesForOneDay(SkipList *list, int date);
QuadNode* getActivitiesEarlier(SkipList* list, int time);
void printSkipList(SkipList* list);

int main(int argc, char** args){
    if(argc < 2){
        printf("No input file given\n");
        exit(1);
    }
    SkipList list;
    list.head = NULL;
    list.maxHeight = 0;
    readInput(args[1], &list);
    return 0;
}

void readInput(char* filename, SkipList* list){
    FILE *inputFile = fopen(filename, "r");

    if(inputFile == NULL){
        printf("Couldn't open file\n");
        exit(1);
    }

    //For each line in the given file extract and run the given command
    const int LINE_SIZE = 128;
    char *currentLine = malloc(LINE_SIZE * sizeof(char));
    while (fgets(currentLine, LINE_SIZE - 1, inputFile) != NULL) {
        // First call to strtok returns the first token
        char *token = strtok(currentLine, " ");

        char *firstToken = malloc(strlen(token) + 1);
        strcpy(firstToken, token);

        //Remove newlines. I use this because it eliminates newlines anywhere in the string instead of just the end
        firstToken[strcspn(firstToken, "\n\r")] = 0;
        printf("%s", firstToken);

        // Move strtok to the next token and remove newlines
        token = strtok(NULL, " ");
        if(token != NULL){
            token[strcspn(token, "\n\r")] = 0;
        }

        if(firstToken[0] == 'P'){ //PrintSkipList
            printf("\n");
            printSkipList(list);
        } else if(firstToken[0] == 'A'){ //AddActivity
            int time = atoi(token);
            printf(" %08d", time);
            //Check if there's already an activity at the given time
            QuadNode* activityAlreadyExists = getActivity(list->head, time);
            if(activityAlreadyExists != NULL && activityAlreadyExists->data->time == time){
                printf(" ExistingActivityError:%s", activityAlreadyExists->data->activityName);
            } else {
                token = strtok(NULL, " ");

                char* activityName = malloc(strlen(token) + 1);
                strcpy(activityName, token);
                activityName[strcspn(activityName, "\n\r")] = 0;
                printf(" %s", activityName);

                addActivity(list, createActivity(activityName, time));
            }
        } else if (firstToken[10] == 'y'){ //GetActivity
            QuadNode* node = getActivity(list->head, atoi(token));
            //If the list is empty or if the node found isn't the one we're looking for, print none
            if(node == NULL || node->data->time != atoi(token)){
                printf(" %s none", token);
            } else {
                printf(" %s %s", token, node->data->activityName);
            }
        } else if(firstToken[13] == 'B' || firstToken[14] == 'o' || firstToken[14] == 'r'){
            //GetActivitiesBetweenTimes, GetActivitiesForOneDay, GetActivitiesFromEarlierInTheDay
            QuadNode* activities;
            int time1 = atoi(token);
            printf(" %s", token);

            if(firstToken[14] == 'o'){ //GetActivitiesForOneDay
                activities = getActivitiesForOneDay(list, time1);
            } else if (firstToken[14] == 'r'){ //GetActivitiesFromEarlierInTheDay
                activities = getActivitiesEarlier(list, time1);
            } else { //GetActivitiesBetweenTimes
                token = strtok(NULL, " ");
                int time2 = atoi(token);
                token[strcspn(token, "\n\r")] = 0;
                printf(" %s", token);

                activities = getActivitiesBetweenTimes(list, time1, time2);
            }

            QuadNode* printNode = activities;
            if(printNode == NULL){
                printf(" none");
            } else {
                while (printNode != NULL){
                    printf(" %08d:%s", printNode->data->time, printNode->data->activityName);
                    printNode = printNode->next;
                }
            }
        } else if (firstToken[0] == 'R'){ //RemoveActivity
            QuadNode* node = getActivity(list->head, atoi(token));
            if(node == NULL || node->data->time != atoi(token)){
                printf(" %s NoActivityError", token);
            } else {
                printf(" %s %s", token, node->data->activityName);
                removeActivity(list, atoi(token));
            }
        }
        printf("\n");
        free(firstToken);
    }
    fclose(inputFile);
    free(currentLine);
}

Activity* createActivity(char* activityName, int time){
    Activity* activity = malloc(sizeof(Activity));
    activity->activityName = activityName;
    activity->time = time;
    return activity;
}

QuadNode *createSkipNode(Activity* data, QuadNode *prev, QuadNode *next, QuadNode *above, QuadNode *below) {
    QuadNode* newNode = (QuadNode*) malloc(sizeof(QuadNode));
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

void createNodesAtLevelAndBelow(SkipList* list, QuadNode* startingNode, QuadNode* aboveNode, int key, Activity* activity, int numberOfLevels){
    QuadNode* currentLevel = startingNode;
    int j;
    for (j = numberOfLevels; j >= 0; --j) {
        while (key > currentLevel->next->data->time){
            currentLevel = currentLevel->next;
        }
        QuadNode* newNode = createSkipNode(activity, currentLevel, currentLevel->next, aboveNode, NULL);
        currentLevel->next->prev = newNode;
        currentLevel->next = newNode;
        if(aboveNode != NULL){
            aboveNode->below = newNode;
        }
        // Go down now
        aboveNode = newNode;
        currentLevel = currentLevel->below;
    }
}

void addActivity(SkipList *list, Activity* activity) {
    int key = activity->time;
    int height = getRandHeight();

    Activity* minData = malloc(sizeof(Activity));
    minData->time = INT_MIN;
    minData->activityName = NULL;
    Activity* maxData = malloc(sizeof(Activity));
    maxData->time = INT_MAX;
    maxData->activityName = NULL;

    // Initialize list
    if (list->head == NULL) {
        // Create top level of the SkipList
        QuadNode *minNode = createSkipNode(minData, NULL, NULL, NULL, NULL);
        QuadNode *maxNode = createSkipNode(maxData, minNode, NULL, NULL, NULL);
        QuadNode* newNode = createSkipNode(activity, minNode, maxNode, NULL, NULL);

        minNode->next = newNode;
        maxNode->prev = newNode;

        list->head = minNode;

        // Create all the levels below the top level
        QuadNode *prevNode = list->head;
        int i;
        for (i = 0; i < height; ++i) {
            QuadNode *newMinNode = createSkipNode(minData, NULL, NULL, prevNode, NULL);
            QuadNode *node = createSkipNode(activity, newMinNode, NULL, prevNode->next, NULL);
            QuadNode *newMaxNode = createSkipNode(maxData, node, NULL, prevNode->next->next, NULL);
            node->next = newMaxNode;
            newMinNode->next = node;

            prevNode->below = newMinNode;
            prevNode->next->below = node;
            prevNode->next->next->below = newMaxNode;

            prevNode = newMinNode;
        }
        list->maxHeight = height;
    }
    else if (height > list->maxHeight) {
        QuadNode* oldHead = list->head;

        // Copied the code for making the initial skip list so all we need to do is connect it
        // Create top level
        QuadNode *topMinNode = createSkipNode(minData, NULL, NULL, NULL, NULL);
        QuadNode *topMaxNode = createSkipNode(maxData, topMinNode, NULL, NULL, NULL);
        QuadNode *topNewNode = createSkipNode(activity, topMinNode, topMaxNode, NULL, NULL);

        topMinNode->next = topNewNode;
        topMaxNode->prev = topNewNode;

        list->head = topMinNode;

        // Create and link the levels between the new maxHeight and the old maxHeight
        QuadNode *aboveMinNode = list->head;
        QuadNode *aboveMaxNode = list->head->next->next;
        QuadNode *node = topNewNode;
        int i;
        for (i = 0; i < height - list->maxHeight - 1; ++i) {
            QuadNode *newMinNode = createSkipNode(minData, NULL, NULL, aboveMinNode, NULL);
            node = createSkipNode(activity, newMinNode, NULL, aboveMinNode->next, NULL);
            QuadNode* newMaxNode = createSkipNode(maxData, node, NULL, aboveMinNode->next->next, NULL);
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
        while (key > searchNode->next->data->time){
            searchNode = searchNode->next;
        }

        // Once we find where this goes lets put it there
        QuadNode* newNode = createSkipNode(activity, searchNode, searchNode->next, node, NULL);
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
        oldHead->above = aboveMinNode;
        tempNode->above = aboveMaxNode;

        // Add and link up the new node in the rest of the lists below
        // We start looking below the node we just inserted after
        createNodesAtLevelAndBelow(list, searchNode->below, newNode, key, activity, list->maxHeight - 1);
        list->maxHeight = height;
    } else if (height == list->maxHeight){
        //Insert the new node on the current level and build the ones below it
        createNodesAtLevelAndBelow(list, list->head, NULL, key, activity, list->maxHeight);
    } else {
        // If the new node is less than the maxHeight
        QuadNode* aboveNode = NULL;
        QuadNode* currentLevel = list->head;
        int i;
        for (i = 0; i < list->maxHeight - height; ++i) {
            currentLevel = currentLevel->below;
        }
        createNodesAtLevelAndBelow(list, currentLevel, aboveNode, key, activity, height);
    }
}

QuadNode* getActivity(QuadNode* list, int key){
    if(list == NULL){
        return NULL;
    }
    // Search left to right and drop down if not in the current list
    QuadNode* searchNode = list;
    while (searchNode->next != NULL){
        if(key == searchNode->next->data->time){
            return searchNode->next;
        }
        if(key < searchNode->next->data->time){
            if(searchNode->below != NULL){
                searchNode = searchNode->below;
                continue;
            } else {
                // Break and return the current/closest node if the key isn't in the list
                // This makes this function return the floorEntry of key if it isn't found
                break;
            }
        }
        searchNode = searchNode->next;
    }
    return searchNode;
}

void removeActivity(SkipList* list, int key){
    QuadNode* nodeToDelete = getActivity(list->head, key);
    // Return if it isn't in the list
    if(nodeToDelete == NULL || nodeToDelete->data->time != key){
        return;
    }
    while (nodeToDelete != NULL){
        // Delete level if the node we're deleting is its only element
        if(nodeToDelete->prev->data->time == INT_MIN && nodeToDelete->next->data->time == INT_MAX){
            // If this is the last node in the last list, delete the list
            if(nodeToDelete->below == NULL){
                free(nodeToDelete->data->activityName);
                free(nodeToDelete->data);
                free(nodeToDelete->prev);
                free(nodeToDelete->next);
                free(nodeToDelete);
                list->head = NULL;
                list->maxHeight = 0;
                break;
            }
            nodeToDelete->prev->below->above = NULL;
            nodeToDelete->next->below->above = NULL;

            list->head = nodeToDelete->prev->below;
            QuadNode* belowNode = nodeToDelete->below;
            free(nodeToDelete->prev);
            free(nodeToDelete->next);
            free(nodeToDelete);
            nodeToDelete = belowNode;
            list->maxHeight--;
            continue;
        }
        // Otherwise just remove the node from the current level
        nodeToDelete->prev->next = nodeToDelete->next;
        nodeToDelete->next->prev = nodeToDelete->prev;
        QuadNode* belowNode = nodeToDelete->below;
        free(nodeToDelete);
        nodeToDelete = belowNode;
    }
}

QuadNode* getActivitiesBetweenTimes(SkipList *list, int key1, int key2){
    if(list->head == NULL){
        return NULL;
    }

    QuadNode* returnList = NULL;

    // We use getActivity to find the node at key1 or before it if it doesn't exist then we go to the bottom level.
    QuadNode* bottomMinNode = getActivity(list->head, key1);
    while (bottomMinNode->below != NULL){
        bottomMinNode = bottomMinNode->below;
    }

    // Add the nodes between k1 and k2 to a doubly linked list of QuadNodes without nodes above or below them.
    QuadNode* searchNode = bottomMinNode;
    while (searchNode->next != NULL && searchNode->data->time <= key2){
        QuadNode* returnListTempNode = returnList;
        if(searchNode->data->time >= key1){
            QuadNode* newNode = createSkipNode(searchNode->data, returnListTempNode, NULL, NULL, NULL);
            if(returnList == NULL){
                returnList = newNode;
            } else {
                while (returnListTempNode->next != NULL){
                    returnListTempNode = returnListTempNode->next;
                }
                returnListTempNode->next = newNode;
            }
        }
        searchNode = searchNode->next;
    }
    return returnList;
}

QuadNode* getActivitiesForOneDay(SkipList *list, int date){
    // * 10000 turns a date into the MMDDHHmm format. + 2359 is the end of the day
    return getActivitiesBetweenTimes(list, date * 10000, date * 10000 + 2359);
}

QuadNode* getActivitiesEarlier(SkipList* list, int time){
    // This gets the first minute of the day. The / and * don't cancel because of how integer math works
    if(time % 10000 == 0){
        return NULL;
    }
    //time - 1 makes it so this function isn't inclusive
    return getActivitiesBetweenTimes(list, time / 10000 * 10000, time - 1);
}

void printSkipList(SkipList *list) {
    //If the SkipList is empty, just print S0 is empty
    if(list->head == NULL){
        printf("(S0) empty\n");
        return;
    }

    QuadNode *tempNode = list->head;
    // To keep track of the level we're on
    int counter = list->maxHeight;

    // Print each level like "(S0) time1:activity1 ...."
    printf("(S%d) empty\n", counter + 1);
    while (tempNode != NULL){
        printf("(S%d)", counter);
        QuadNode* levelTempNode = tempNode;
        while (levelTempNode->next != NULL){
            //Print all nodes that aren't min/max
            if(levelTempNode->data->time != INT_MIN && levelTempNode->data->time != INT_MAX){
                printf(" %08d:%s",  levelTempNode->data->time,levelTempNode->data->activityName);
            }
            levelTempNode = levelTempNode->next;
        }
        printf("\n");
        counter--;
        tempNode = tempNode->below;
    }
}