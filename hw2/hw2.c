#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SectionNode {
    char course[10];
    char day[4];
    char time[5];
    int preference;
    struct SectionNode *next;
} Section;

void addNode(char[10], char[4], char[5], int, Section **);
void addExistingNode(Section*, Section**);
void cloneLinkedList(Section **newList, Section *oldList);
void removeNode(Section* nodeToRemove, Section** list);
int nodesEqual(Section *node1, Section *node2);
int overlap(Section* node1, Section* node2);
void removeConflicts(Section **schedule, Section **conflicts);
void checkSchedule(Section** schedule);
void makeSchedule(Section *known, Section *unknown, int numUnknown, Section *returnArray);
void readInput(char*, Section**, int*);
int courseInSchedule(Section* node, Section* schedule);
void freeLinkedList(Section** list);

int lowestPrefSum = -1;
int mostCourses = -1;

Section *bestSchedule = NULL;
Section *timeConflicts = NULL;

int main(int argc, char **args) {
    // Check if the user supplied a input file
    if (argc == 1) {
        printf("No input file specified");
        return 1;
    }
    Section* courseList = NULL;
    int numCourses;
    readInput(args[1], &courseList, &numCourses);
    /*addNode("TR", "1100", "1", 0, &courseList);
    addNode("TR", "1100", "2", 1, &courseList);
    addNode("TR", "1100", "3", 2, &courseList);
    numCourses = 3;*/

//    checkSchedule(&courseList);
    makeSchedule(NULL, courseList, numCourses, NULL);

    printf("---Course schedule---\n");
    Section *bestTempNode = bestSchedule;
    while (bestTempNode != NULL) {
        printf("%s %s%s\n", bestTempNode->course, bestTempNode->day, bestTempNode->time);
        bestTempNode = bestTempNode->next;
    }

    printf("---Courses with a time conflict---\n");
    Section *conflictsTempNode = timeConflicts;
    while (conflictsTempNode != NULL) {
        printf("%s %s%s\n", conflictsTempNode->course, conflictsTempNode->day, conflictsTempNode->time);
        conflictsTempNode = conflictsTempNode->next;
    }
}

void makeSchedule(Section *known, Section *unknown, int numUnknown, Section *returnArray) {
    Section *tempUnknown = NULL;
    cloneLinkedList(&tempUnknown, unknown);
    Section *tempNode = unknown;

    while (tempNode != NULL) {
        //Remove tempNode from tempUnknown
        removeNode(tempNode, &tempUnknown);

        //Add tempNode to the end of known
        addExistingNode(tempNode, &known);

        if (numUnknown == 1) {
            Section* schedule = NULL;
            cloneLinkedList(&schedule, known);
            checkSchedule(&schedule);
        } else {
            makeSchedule(known, tempUnknown, numUnknown - 1, returnArray);
        }

        //Add tempNode back to tempUnknown
        addExistingNode(tempNode, &tempUnknown);

        //Remove tempNode from the end of known
        removeNode(tempNode, &known);
        tempNode = tempNode->next;
    }
}

void checkSchedule(Section** schedule) {
    Section *conflicts = NULL;
    removeConflicts(schedule, &conflicts);
    if(bestSchedule == NULL){
        cloneLinkedList(&bestSchedule, *schedule);
        cloneLinkedList(&timeConflicts, conflicts);
        freeLinkedList(schedule);
        freeLinkedList(&conflicts);
        return;
    }
    int bestPrefSum = 0;
    int currentPrefSum = 0;
    int bestNumCourses = 0;
    int currentNumCourses = 0;

    Section *bestTempNode = bestSchedule;
    while (bestTempNode != NULL) {
        bestNumCourses += 1;
        bestPrefSum += bestTempNode->preference;
        bestTempNode = bestTempNode->next;
    }

    Section *scheduleTempNode = *schedule;
    while (scheduleTempNode != NULL) {
        currentNumCourses += 1;
        currentPrefSum += scheduleTempNode->preference;
        scheduleTempNode = scheduleTempNode->next;
    }

    if (currentNumCourses > bestNumCourses ||
        (currentNumCourses == bestNumCourses && currentPrefSum < bestPrefSum)) {
        bestSchedule = *schedule;
        timeConflicts = conflicts;
    }
}

void addNode(char day[4], char time[5], char course[10], int preference, Section **head) {
    Section *newNode = malloc(sizeof(Section));
    strcpy(newNode->day, day);
    strcpy(newNode->time, time);
    strcpy(newNode->course, course);
    newNode->preference = preference;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        Section *tempNode = *head;
        while (tempNode->next != NULL) {
            tempNode = tempNode->next;
        }
        tempNode->next = newNode;
    }
}

void addExistingNode(Section* node, Section** list){
    addNode(node->day, node->time, node->course, node->preference, list);
}

void cloneLinkedList(Section **newList, Section *oldList) {
    Section *tempNode = oldList;
    while (tempNode != NULL) {
        addNode(tempNode->day, tempNode->time, tempNode->course, tempNode->preference, newList);
        tempNode = tempNode->next;
    }
}

void freeLinkedList(Section** list){
    Section *tempNode = *list;
    while (tempNode != NULL) {
        Section* next = tempNode->next;
        free(tempNode);
        tempNode = next;
    }
}

void removeNode(Section* nodeToRemove, Section** list){
    // If we have to remove the head
    if((*list) != NULL && nodesEqual(nodeToRemove, *list)){
        Section* currentHead = *list;
        *list = (*list)->next;
        free(currentHead);
    } else if((*list)->next != NULL && nodesEqual((*list)->next, nodeToRemove)){
        Section* currentNext = (*list)->next;
        (*list)->next = (*list)->next->next;
        free(currentNext);
    } else if ((*list)->next->next != NULL){
        Section* tempNode = (*list);
        while (tempNode->next->next != NULL && !nodesEqual(tempNode->next->next, nodeToRemove)){
            tempNode = tempNode->next;
        }
        free(tempNode->next);
        tempNode->next = tempNode->next->next;
    }
}

int overlap(Section* node1, Section* node2) {
    char* class1days = node1->day;
    char* class2days = node2->day;
    int overlap = 0;
    int i;
    for(i = 0; i < strlen(class1days); i++){
        int j;
        for (j = 0; j < strlen(class2days); j++) {
            if(class1days[i] == class2days[j]){
                int class1 = atoi(node1->time);
                int class2 = atoi(node2->time);
                if(class1days[i] == 'M' || class1days[i] == 'W' || class1days[i] == 'F'){
                    if (class2 < class1 + 100 && class2 >= class1 - 100) {
                        return 1;
                    }
                }
                if(class1days[i] == 'T' || class1days[i] == 'R'){
                    if (class2 < class1 + 130 && class2 >= class1 - 130) {
                        return 1;
                    }
                }
            }
        }
    }
    return overlap;
}

int nodesEqual(Section *node1, Section *node2) {
    if (strcmp(node1->course, node2->course) == 0
        && strcmp(node1->day, node2->day) == 0
        && strcmp(node1->time, node2->time) == 0) {
        return 1;
    }
    return 0;
}

void removeConflicts(Section **schedule, Section **conflicts) {
    Section *tempNode = *schedule;
    while (tempNode != NULL) {
        Section *prevSearchTempNode = tempNode;
        Section *searchTempNode = tempNode->next;
        while (searchTempNode != NULL) {
            if (overlap(tempNode, searchTempNode)) {
                Section* tempNext = searchTempNode->next;
                if(!courseInSchedule(searchTempNode, *schedule)){
                    addExistingNode(searchTempNode, conflicts);
                }
                removeNode(searchTempNode, &prevSearchTempNode);
                searchTempNode = tempNext;
                continue;
            }
            prevSearchTempNode = searchTempNode;
            searchTempNode = searchTempNode->next;
        }
        tempNode = tempNode->next;
    }
}

void readInput(char* fileName, Section** list, int* numCourses){
    // Open the file
    FILE *inputFile = fopen(fileName, "r");

    char *course = malloc(sizeof(char) * 32);
    int preference = 0;

    while (fgets(course, 100, inputFile) != NULL) {
        // First call to strtok returns the first token
        char *token = strtok(course, " ");
        char *courseName = malloc(strlen(token));
        strcpy(courseName, token);
        // Move strtok to the first course time
        token = strtok(NULL, " ");

        char *dayString = calloc(1, strlen(token));
        char *timeString = calloc(1, 4);
        while (token != NULL) {
            int dayStringIndex = 0;
            int timeIndex = 0;
            int i;
            for (i = 0; i < strlen(token); ++i) {
                // Find and ignore newlines from the tokens
                if (token[i] != '\r' && token[i] != '\n') {
                    if (token[i] >= 48 && token[i] <= 57) {
                        timeString[timeIndex] = token[i];
                        timeIndex++;
                    } else {
                        dayString[dayStringIndex] = token[i];
                        dayStringIndex++;
                    }
                }
            }
            addNode(dayString, timeString, courseName, preference, list);

            preference++;
            // strtok returns the next token in the token list when supplied with NULL
            token = strtok(NULL, " ");
        }
        free(dayString);
        free(timeString);
        free(courseName);
    }
    free(course);
    // The preference at the end of the loop ends up being the length of the linked list
    *numCourses = preference;
}

int courseInSchedule(Section* node, Section* schedule){
    Section* tempNode = schedule;
    while (tempNode != NULL){
        if(strcmp(tempNode->course, node->course) == 0 && !nodesEqual(tempNode, node)){
            return 1;
        }
        tempNode = tempNode->next;
    }
    return 0;
}