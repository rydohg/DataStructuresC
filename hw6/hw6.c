/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:

*/
#include "hw6.h"
void readInput(char* filename, Graph* graph);
void addGraphNode(char data, Graph *graph);
void addAdjNode(GraphNode* node, int adjNodeIndex);
void printMaze(Graph* maze);

int main(int argc, char** args){
    if(argc < 2){
        printf("No input\n");
        exit(1);
    }
    Graph maze;
    maze.graphArray = NULL;
    maze.rowLength = 0;
    maze.colHeight = 0;
    maze.emptyIndex = 0;

    readInput(args[1], &maze);

    // Turn the graph's array into a traditional array so it can be represented in CLion's debugger
    GraphNode* debugArray[maze.rowLength * maze.colHeight];
    for (int i = 0; i < maze.rowLength * maze.colHeight; ++i) {
        debugArray[i] = maze.graphArray[i];
    }
    printMaze(&maze);
    printf("idk");
    return 0;
}

void printMaze(Graph* maze) {
    for (int i = 0; i < maze->rowLength * maze->colHeight; ++i) {
        printf("%c", maze->graphArray[i]->data);
        // Add a newline after the last char in  a row
        if(i % maze->rowLength == maze->rowLength - 1){
            printf("\n");
        }
    }
}

void readInput(char* filename, Graph* graph){
    FILE *inputFile = fopen(filename, "r");

    if(inputFile == NULL){
        printf("Couldn't open file\n");
        exit(1);
    }
    // Add each line to the return string
    int lineCounter = 0;
    const int LINE_SIZE = 32;
    char *currentLine = malloc(LINE_SIZE * sizeof(char) + 1);
    while (fgets(currentLine, LINE_SIZE, inputFile) != NULL) {
        // Remove newlines from the end of the line.
        // This removes the first instance of \n or \r
        currentLine[strcspn(currentLine, "\n\r")] = 0;
        if(lineCounter == 0){
            char* mazeSizeString = strtok(currentLine, " ");
            graph->rowLength = atoi(mazeSizeString);
            graph->colHeight = atoi(strtok(NULL, " "));
            graph->graphArray = calloc(graph->rowLength * graph->colHeight, sizeof(GraphNode*));
        } else {
            for (int i = 0; i < strlen(currentLine); ++i) {
                addGraphNode(currentLine[i], graph);
            }
        }
        lineCounter++;
    }
    fclose(inputFile);
    free(currentLine);
}

void addGraphNode(char data, Graph *graph) {
    GraphNode* newNode = malloc(sizeof(GraphNode));
    newNode->data = data;
    newNode->adjNodes = NULL;
    graph->graphArray[graph->emptyIndex] = newNode;

    if(graph->emptyIndex == 0){
        graph->emptyIndex = 1;
        return;
    }

    if(graph->emptyIndex >= graph->rowLength * graph->colHeight){
        printf("Input maze is wrong\n");
        exit(1);
    }

    // If the new node isn't in the first row
    if(graph->emptyIndex / graph->rowLength != 0){
        // Make this node and the one above it adjacent to each other
        addAdjNode(graph->graphArray[graph->emptyIndex], graph->emptyIndex - graph->rowLength);
        addAdjNode(graph->graphArray[graph->emptyIndex - graph->rowLength], graph->emptyIndex);
    }

    // If the node isn't at the start of a row
//    if(graph->emptyIndex % (graph->rowLength + 1) < graph->rowLength){
    if(graph->emptyIndex % graph->rowLength != 0){
        // Make this node and the one before it adjacent to each other
        addAdjNode(graph->graphArray[graph->emptyIndex], graph->emptyIndex - 1);
        addAdjNode(graph->graphArray[graph->emptyIndex - 1], graph->emptyIndex);
    }
    graph->emptyIndex++;
}

//TODO: This can be reworked into having node->adjNodes be an array of size 4
void addAdjNode(GraphNode* node, int adjNodeIndex) {
    AdjacentNode* adjacentNode = malloc(sizeof(AdjacentNode));
    adjacentNode->adjNodeIndex = adjNodeIndex;

    if(node->adjNodes == NULL){
        node->adjNodes = adjacentNode;
    } else {
        //TODO: Keep this sorted by adding in order (up, left, right, down)
        AdjacentNode* tempNode = node->adjNodes;
        while (tempNode->next != NULL){
            tempNode = tempNode->next;
        }
        tempNode->next = adjacentNode;
    }
}
