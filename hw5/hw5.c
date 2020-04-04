/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:
*/
#include "hw5.h"

int main(int argc, char** args){
/*    if(argc < 2){
        printf("No input file given\n");
        exit(1);
    }*/
//    readInput(args[1]);
    SkipList list;
    list.head = NULL;
    list.maxHeight = 0;
    skipListInsert(&list, createActivity("Test", 1));
    skipListInsert(&list, createActivity("Test", 2));
    skipListInsert(&list, createActivity("Test", 3));
    skipListInsert(&list, createActivity("Test", 4));
    skipListInsert(&list, createActivity("Test", 6));
    skipListInsert(&list, createActivity("Test", 8));
    skipListInsert(&list, createActivity("Test", 9));
    skipListInsert(&list, createActivity("Test", 10));
    printSkipList(&list);
    return 0;
}

void readInput(char* filename){
    FILE *inputFile = fopen(filename, "r");

    const int LINE_SIZE = 128;

    char *currentLine = malloc(LINE_SIZE * sizeof(char));
    while (fgets(currentLine, LINE_SIZE - 1, inputFile) != NULL) {
        // First call to strtok returns the first token
        char *token = strtok(currentLine, " ");

        char *firstToken = malloc(strlen(token) + 1);
        strcpy(firstToken, token);

        //Remove newlines
        firstToken[strcspn(firstToken, "\n")] = 0;

        // Move strtok to the next token
        token = strtok(NULL, " ");

        while (token != NULL) {

            token = strtok(NULL, " ");
        }
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

void printSkipList(SkipList *list) {
    if(list->head == NULL){
        printf("SkipList is NULL\n");
        return;
    }
    QuadNode *tempNode = list->head; // current node

    int counter = list->maxHeight;
    while (tempNode != NULL){
        printf("Level %d: ", counter);
        QuadNode* levelTempNode = tempNode;
        while (levelTempNode->next != NULL){
            if(levelTempNode->data->time != INT_MIN && levelTempNode->data->time != INT_MAX){
                printf("%d ", levelTempNode->data->time);
            }
            levelTempNode = levelTempNode->next;
        }
        printf("\n");
        counter--;
        tempNode = tempNode->below;
    }
}

void skipListInsert(SkipList *list, Activity* activity) {
    int key = activity->time;
    QuadNode *new_node = NULL;   // node to be inserted

    int height = getRandHeight();

    Activity* minData = malloc(sizeof(Activity));
    minData->time = INT_MIN;
    minData->activityName = NULL;
    Activity* maxData = malloc(sizeof(Activity));
    maxData->time = INT_MAX;
    maxData->activityName = NULL;

    // empty list
    if (list->head == NULL) {
        QuadNode *minNode = createSkipNode(minData, NULL, NULL, NULL, NULL);
        QuadNode *maxNode = createSkipNode(maxData, minNode, NULL, NULL, NULL);
        new_node = createSkipNode(activity, minNode, maxNode, NULL, NULL);

        minNode->next = new_node;
        maxNode->prev = new_node;

        list->head = minNode;

        QuadNode *prev_node = list->head;
        for (int i = 0; i < height; ++i) {
            //TODO: rename
            QuadNode *newMinNode = createSkipNode(minData, NULL, NULL, prev_node, NULL);
            QuadNode *node = createSkipNode(activity, newMinNode, NULL, prev_node->next, NULL);
            QuadNode *newMaxNode = createSkipNode(maxData, node, NULL, prev_node->next->next, NULL);
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
        QuadNode *topMinNode = createSkipNode(minData, NULL, NULL, NULL, NULL);
        QuadNode *topMaxNode = createSkipNode(maxData, topMinNode, NULL, NULL, NULL);
        QuadNode *topNewNode = createSkipNode(activity, topMinNode, topMaxNode, NULL, NULL);

        topMinNode->next = topNewNode;
        topMaxNode->prev = topNewNode;

        list->head = topMinNode;

        QuadNode *aboveMinNode = list->head;
        QuadNode *aboveMaxNode = list->head->next->next;
        QuadNode *node = topNewNode;
        for (int i = 0; i < height - list->maxHeight - 1; ++i) {
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
        QuadNode* aboveNode;
        QuadNode* currentLevel = searchNode->below;
        for (int j = list->maxHeight - 1; j >= 0; --j) {
            while (key > currentLevel->next->data->time){
                currentLevel = currentLevel->next;
            }
            aboveNode = newNode;
            newNode = createSkipNode(activity, currentLevel, currentLevel->next, aboveNode, NULL);
            currentLevel->next->prev = newNode;
            currentLevel->next = newNode;
            aboveNode->below = newNode;

            //Go down now
            currentLevel = currentLevel->below;
        }
        list->maxHeight = height;
    } else if (height == list->maxHeight){
        QuadNode* searchNode = list->head;
        while (key > searchNode->next->data->time){
            searchNode = searchNode->next;
        }
        QuadNode* newNode = createSkipNode(activity, searchNode, searchNode->next,NULL, NULL);
        searchNode->next->prev = newNode;
        searchNode->next = newNode;

        QuadNode* aboveNode = newNode;
        QuadNode* currentLevel = searchNode->below;
        for (int j = list->maxHeight - 1; j >= 0; --j) {
            while (key > currentLevel->next->data->time){
                currentLevel = currentLevel->next;
            }
            newNode = createSkipNode(activity, currentLevel, currentLevel->next, aboveNode, NULL);
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
            while (key > currentLevel->next->data->time){
                currentLevel = currentLevel->next;
            }
            QuadNode* newNode = createSkipNode(activity, currentLevel, currentLevel->next, aboveNode, NULL);
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

QuadNode *createSkipNode(Activity* data, QuadNode *prev, QuadNode *next, QuadNode *above, QuadNode *below) {
    QuadNode* newNode = (QuadNode *) malloc(sizeof(QuadNode));
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
