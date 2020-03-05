/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2120
  Section: 14

  Description of this file:
    Creates a tree of sports, events, and athletes from an input data file
    then run queries from another input file on the tree
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// The tree consists of a link to its parent, rightSibling, and its leftmost child
// it stores its data in a char*
typedef struct Node {
    struct Node *parent;
    struct Node *rightSibling;
    struct Node *children;
    char *data;
} TreeNode;

// I'm using this as a way of easily traversing athletes or countries
typedef struct LNode {
    TreeNode *data;
    int numOfMedals;
    struct LNode *next;
} ListNode;

void readInput(int, char **);
void insertChild(TreeNode **, char *);
void appendChild(TreeNode **, char *);
void findNode(char *, TreeNode *, TreeNode **);
void printChildren(TreeNode *);
void getListOfAthletesOrCountries(TreeNode *, int, int, int, ListNode **);
void addNode(TreeNode *, int, ListNode **);
char *getCountryFromString(char *);
char *getAthleteFromString(char *);

//Globally store the tree
TreeNode *root = NULL;

int main(int argc, char **args) {
    readInput(argc, args);
    return 0;
}

void readInput(int argc, char **args) {
    if (argc < 3) {
        printf("Missing an input file");
        exit(1);
    }
    FILE *inputDataFile = fopen(args[1], "r");

    char *currentLine = malloc(128 * sizeof(char));
    while (fgets(currentLine, 128 - 1, inputDataFile) != NULL) {
        // First call to strtok returns the first token
        char *token = strtok(currentLine, " ");
        char *firstToken = malloc(strlen(token));
        strcpy(firstToken, token);

        firstToken[strcspn(firstToken, "\n")] = 0;

        //Initialize tree or find the parent of this token
        TreeNode *parentNode = NULL;
        if (root == NULL) {
            insertChild(&root, firstToken);
            parentNode = root;
        } else {
            findNode(token, root, &parentNode);
        }

        // Move strtok to the next token
        token = strtok(NULL, " ");

        while (token != NULL) {
            char *newData = malloc(strlen(token));
            strcpy(newData, token);

            //Remove newline
            newData[strcspn(newData, "\n")] = 0;

            /* A node is a name if it contains a semicolon
             and we dont care about alphabetical order for
             athletes so we use appendChild instead of
             insertChild which does care about order
             */
            int i, isName = 0;
            for (i = 0; i < strlen(newData); ++i) {
                if (newData[i] == ':') { isName = 1; }
            }
            if (isName) {
                appendChild(&parentNode, newData);
            } else {
                insertChild(&parentNode, newData);
            }

            token = strtok(NULL, " ");
        }
    }
    fclose(inputDataFile);
    free(currentLine);

    FILE *inputQueryFile = fopen(args[2], "r");

    currentLine = malloc(128 * sizeof(char));
    while (fgets(currentLine, 128 - 1, inputQueryFile) != NULL) {
        //This piece of code removes the newline from the token
        char *token = strtok(currentLine, " ");
        token[strcspn(token, "\n")] = 0;

        char *command = malloc(strlen(token));
        strcpy(command, token);

        // Each outout line begins with the command
        printf("%s ", command);
        if (strcmp(token, "GetEventsBySport") == 0) {
            // Find a node then print its children
            token = strtok(NULL, " ");
            token[strcspn(token, "\n")] = 0;

            TreeNode *returnNode = NULL;
            findNode(token, root, &returnNode);

            printf("%s", token);
            printChildren(returnNode);
        } else if (strcmp(token, "GetWinnersAndCountriesBySportAndEvent") == 0 ||
                   strcmp(token, "GetGoldMedalistAndCountryBySportAndEvent") == 0) {
            // Find the node for the sport and event then selectively print
            // out all children for medals or just the gold medal winner
            token = strtok(NULL, " ");
            char *sport = malloc(strlen(token));
            strcpy(sport, token);

            token = strtok(NULL, " ");
            token[strcspn(token, "\n")] = 0;

            char *event = malloc(strlen(token));
            strcpy(event, token);

            TreeNode *sportNode = NULL;
            TreeNode *eventNode = NULL;
            findNode(sport, root, &sportNode);
            findNode(event, sportNode, &eventNode);

            //If its Winners, otherwise its GoldMedalist
            if (command[3] == 'W') {
                printf("%s %s", sport, event);
                printChildren(eventNode);
            } else {
                printf("%s %s %s", sport, event, eventNode->children->data);
            }

            free(sport);
            free(event);
        } else if (strcmp(token, "GetAthleteWithMostMedals") == 0
                   || strcmp(token, "GetAthleteWithMostGoldMedals") == 0
                   || strcmp(token, "GetCountryWithMostMedals") == 0
                   || strcmp(token, "GetCountryWithMostGoldMedals") == 0) {
            /*
             * This is a creative way of reusing code. Since the code for all 4 queries are essentially the same,
             * we just have the getListOfAthletesOrCountries function change its return list then we just find mostMedals
             * for all queries
             */
            int onlyCountries = 0;
            ListNode *list = NULL;
            if (strcmp(token, "GetAthleteWithMostMedals") == 0) {
                //Get all athletes
                getListOfAthletesOrCountries(root, 0, 0, 0, &list);
            } else if (strcmp(token, "GetAthleteWithMostGoldMedals") == 0) {
                //Get only gold medal winning athletes
                getListOfAthletesOrCountries(root, 1, 0, 0, &list);
            } else if (strcmp(token, "GetCountryWithMostMedals") == 0) {
                //Get all countries
                onlyCountries = 1;
                getListOfAthletesOrCountries(root, 0, 1, 0, &list);
            } else if (strcmp(token, "GetCountryWithMostGoldMedals") == 0) {
                //Get only countries that won gold medals
                onlyCountries = 1;
                getListOfAthletesOrCountries(root, 1, 1, 0, &list);
            }

            //Loop through list to find most medals and add it to mostMedalsList
            int mostMedals = 0;
            ListNode *mostMedalsList = NULL;
            ListNode *currentAthleteOrCountry = list;
            while (currentAthleteOrCountry != NULL) {
                if (currentAthleteOrCountry->numOfMedals > mostMedals) {
                    free(mostMedalsList);
                    mostMedalsList = NULL;
                }
                if (currentAthleteOrCountry->numOfMedals >= mostMedals) {
                    addNode(currentAthleteOrCountry->data, currentAthleteOrCountry->numOfMedals, &mostMedalsList);
                    mostMedals = currentAthleteOrCountry->numOfMedals;
                }
                currentAthleteOrCountry = currentAthleteOrCountry->next;
            }

            // We use safeData because getXfromString destroys the original string
            // Loop through and print out most medals and the athletes/countries that have that many
            printf("%d", mostMedals);
            ListNode *tempPrintNode = mostMedalsList;
            while (tempPrintNode != NULL) {
                char *safeData = malloc(sizeof(tempPrintNode->data->data));
                strcpy(safeData, tempPrintNode->data->data);
                char *data = !onlyCountries ? getAthleteFromString(safeData) : getCountryFromString(
                        safeData);
                printf(" %s", data);
                tempPrintNode = tempPrintNode->next;
            }
        } else if (strcmp(token, "GetSportAndEventByAthlete") == 0) {
            //Get all athletes including duplicates then navigate up the tree to find the sport and event names
            token = strtok(NULL, " ");
            token[strcspn(token, "\n")] = 0;

            ListNode *athletes = NULL;
            getListOfAthletesOrCountries(root, 0, 0, 1, &athletes);

            //Find the events the given athlete is in
            ListNode *athletesEvents = NULL;
            ListNode *athlete = athletes;
            while (athlete != NULL) {
                //Copying the data because the getAthleteFromString function changes the string
                char *data = athlete->data->data;
                char *safeData = malloc(strlen(athlete->data->data));
                strcpy(safeData, data);

                //Adding events to the head of the list puts it in the right order
                if (strcmp(getAthleteFromString(safeData), token) == 0) {
                    ListNode *newNode = malloc(sizeof(ListNode *));
                    newNode->data = athlete->data;
                    newNode->numOfMedals = athlete->numOfMedals;

                    if (athletesEvents == NULL) {
                        newNode->next = NULL;
                        athletesEvents = newNode;
                    } else {
                        newNode->next = athletesEvents;
                        athletesEvents = newNode;
                    }
                }
                free(safeData);
                athlete = athlete->next;
            }

            // Print sports and events
            printf("%s", token);
            ListNode *childOfEvent = athletesEvents;
            while (childOfEvent != NULL) {
                printf(" %s:%s", childOfEvent->data->parent->parent->data, childOfEvent->data->parent->data);
                childOfEvent = childOfEvent->next;
            }
            free(athletes);
        }
        printf("\n");
    }
    fclose(inputQueryFile);
}

// Use recursion to find the node containing *data then return it to returnNode 
void findNode(char *data, TreeNode *node, TreeNode **returnNode) {
    if (node == NULL) {
        return;
    }
    if (strcmp(data, node->data) == 0) {
        *returnNode = node;
    }

    if (node->rightSibling != NULL) {
        findNode(data, node->rightSibling, returnNode);
    }

    if (node->children != NULL) {
        findNode(data, node->children, returnNode);
    }
}

// Alphabetically add nodes to the tree
void insertChild(TreeNode **parentNode, char *newData) {
    //If there's no tree, plant one
    TreeNode *newNode = malloc(sizeof(TreeNode));
    newNode->data = newData;
    newNode->rightSibling = NULL;
    newNode->children = NULL;

    if ((*parentNode) == NULL) {
        newNode->parent = NULL;
        *parentNode = newNode;
    } else if ((*parentNode)->children == NULL) {
        newNode->parent = *parentNode;
        (*parentNode)->children = newNode;
    } else {
        // Check if newData is before the first child and replace it if needed
        if (strcmp(newData, (*parentNode)->children->data) < 0) {
            char *tempData = (*parentNode)->children->data;
            TreeNode *tempRightSibling = (*parentNode)->children->rightSibling;

            (*parentNode)->children->data = newData;

            newNode->data = tempData;
            newNode->parent = *parentNode;
            newNode->rightSibling = tempRightSibling;

            (*parentNode)->children->rightSibling = newNode;
            return;
        }

        // Search through the rest of the children and add the node in alphabetical order
        TreeNode *tempNode = (*parentNode)->children;
        while (tempNode->rightSibling != NULL) {
            if (strcmp(newData, tempNode->rightSibling->data) < 0) {
                newNode->rightSibling = tempNode->rightSibling;
                tempNode->rightSibling = newNode;
                newNode->parent = *parentNode;
                return;
            }
            tempNode = tempNode->rightSibling;
        }
        newNode->parent = *parentNode;
        tempNode->rightSibling = newNode;
    }
}

// Indiscriminately add to the tree
void appendChild(TreeNode **parentNode, char *newData) {
    //If there's no tree, plant one
    TreeNode *newNode = malloc(sizeof(TreeNode));
    newNode->data = newData;
    newNode->rightSibling = NULL;
    newNode->children = NULL;

    if ((*parentNode) == NULL) {
        //Init tree
        newNode->parent = NULL;
        *parentNode = newNode;
    } else if ((*parentNode)->children == NULL) {
        // Make this the parent's first child
        newNode->parent = *parentNode;
        (*parentNode)->children = newNode;
    } else {
        //Make it the rightmost sibling
        TreeNode *tempNode = (*parentNode)->children;
        while (tempNode->rightSibling != NULL) {
            tempNode = tempNode->rightSibling;
        }
        newNode->parent = *parentNode;
        tempNode->rightSibling = newNode;
    }
}

// Use recursion to find all leaf nodes and depending on the arguments, selectively return them
// onlyGold only returns a parents leftmost leaf node (the gold medal winner)
// onlyCountries makes the function that normally finds the number of medals an athlete
// has to just care about each leaf node's country and finds the number of medals a country has
// onlyGold and onlyCountries can work together to find the number of gold medals per country
// includeDuplicates is to make a list with all events per athlete so we can get the sports and events for an athlete
void getListOfAthletesOrCountries(TreeNode *node, int onlyGold, int onlyCountries, int includeDuplicates,
                                  ListNode **returnList) {
    if (node != NULL) {
        int i, isName = 0;
        for (i = 0; i < strlen(node->data); ++i) {
            if (node->data[i] == ':') { isName = 1; }
        }
        if (isName) {
            int alreadyInList = 0;
            ListNode *tempNode = *returnList;
            while (tempNode != NULL) {
                // By only adding the country to the returned list, we can use the same code for 4 queries by
                // just changing the boolean onlyCountries
                char *tempNodeData = !onlyCountries ? tempNode->data->data : getCountryFromString(tempNode->data->data);
                char *nodeData = !onlyCountries ? node->data : getCountryFromString(node->data);

                if (strcmp(tempNodeData, nodeData) == 0) {
                    tempNode->numOfMedals++;
                    alreadyInList = 1;
                    break;
                }
                tempNode = tempNode->next;
            }
            // If an athlete is already in the list we just increment their medal count
            // but we always add to the list if we're includingDuplicates
            if (!alreadyInList || includeDuplicates) {
                addNode(node, 1, returnList);
            }

            // By returning if it found a name it skips its right siblings only giving the gold medalist
            if (onlyGold) { return; }
        }
        // Traverse tree
        if (node->children != NULL) {
            getListOfAthletesOrCountries(node->children, onlyGold, onlyCountries, includeDuplicates, returnList);
        }
        if (node->rightSibling != NULL) {
            getListOfAthletesOrCountries(node->rightSibling, onlyGold, onlyCountries, includeDuplicates, returnList);
        }
    }
}

void printChildren(TreeNode *node) {
    TreeNode *tempNode = node->children;
    while (tempNode != NULL) {
        printf(" %s", tempNode->data);
        tempNode = tempNode->rightSibling;
    }
}

// Alphabetically add to a linked list
void addNode(TreeNode *data, int numMedals, ListNode **list) {
    ListNode *newNode = malloc(sizeof(ListNode));
    newNode->data = data;
    newNode->numOfMedals = numMedals;
    newNode->next = NULL;

    if (*list == NULL) {
        *list = newNode;
    } else if (strcmp(data->data, (*list)->data->data) < 0) {
        newNode->next = (*list);
        (*list) = newNode;
    } else if ((*list)->next == NULL) {
        (*list)->next = newNode;
    } else {
        ListNode *tempNode = (*list)->next;
        while (tempNode->next != NULL && strcmp(data->data, tempNode->data->data) > 0) {
            tempNode = tempNode->next;
        }
        newNode->next = tempNode->next;
        tempNode->next = newNode;
    }
}

char *getCountryFromString(char *data) {
    int i;
    for (i = 0; i < strlen(data); ++i) {
        if (data[i] == ':') {
            return &data[i + 1];
        }
    }
    return NULL;
}

char *getAthleteFromString(char *data) {
    int i;
    for (i = 0; i < strlen(data); ++i) {
        if (data[i] == ':') {
            data[i] = '\0';
            return data;
        }
    }
    return NULL;
}