/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:

*/
#include "hw6.h"
void readMazeFromFile(char* filename, Graph* graph);
void readInput(Graph* graph);
void addGraphNode(char data, Graph *graph);
void addNodeToList(NodeList** list, int nodeIndex);
void printMaze(Graph* maze);
void moveBugs(Graph *maze);
void enqueue(Queue* queue, int data);
int dequeue(Queue* queue);
int queueIsEmpty(Queue* queue){ return (queue->queue[queue->front % queue->arraySize] != -1) ? 0 : 1; }
void bfs(Graph* graph, int startingNodeIndex);

// Position helpers
int up(int index, int rowLength){ return index - rowLength; }
int down(int index, int rowLength){ return index + rowLength; }
int left(int index, int rowLength){ return (index % rowLength != 0) ? index - 1 : -1; }
int right(int index, int rowLength){ return (index % rowLength != rowLength - 1) ? index + 1 : -1; }

const int NOT_ON_QUEUE = 0;
const int NOT_VISITED = 1;
const int VISITED = 2;

int main(int argc, char** args){
    if(argc < 2){
        printf("No input\n");
        exit(1);
    }
    Graph maze;
    maze.graphArray = NULL;
    maze.bugs = NULL;
    maze.rowLength = 0;
    maze.colHeight = 0;
    maze.emptyIndex = 0;

    readMazeFromFile(args[1], &maze);
    // Turn the graph's array into a traditional array so it can be represented in CLion's debugger
    GraphNode* debugArray[maze.rowLength * maze.colHeight];
    for (int i = 0; i < maze.rowLength * maze.colHeight; ++i) {
        debugArray[i] = maze.graphArray[i];
    }
    printMaze(&maze);
    // game loop
    while (1){
        readInput(&maze);
        printMaze(&maze);
        moveBugs(&maze);
    }
    printf("idk");
    return 0;
}

void readInput(Graph* graph){
    int runAgain = 1;
    while(runAgain) {
        runAgain = 0;
        char *move = malloc(sizeof(char) * 2);
        printf("Please enter your move [u(p), d(own), l(elf), or r(ight)]: ");
        // Char, newline, and \0
        fgets(move, 3, stdin);

        int newIndex;
        switch (move[0]) {
            case 'u':
                newIndex = up(graph->tronIndex, graph->rowLength);
                break;
            case 'd':
                newIndex = down(graph->tronIndex, graph->rowLength);
                break;
            case 'l':
                newIndex = left(graph->tronIndex, graph->rowLength);
                break;
            case 'r':
                newIndex = right(graph->tronIndex, graph->rowLength);
                break;
            default:
                newIndex = -1;
                runAgain = 1;
                break;
        }
        if(newIndex != -1){
            if (graph->graphArray[newIndex]->data == ' ') {
                graph->graphArray[graph->tronIndex]->data = ' ';
                graph->graphArray[newIndex]->data = 'T';
                graph->tronIndex = newIndex;
            } else if (graph->graphArray[newIndex]->data == 'I') {
                printf("You win!\n");
                exit(0);
            } else {
                printf("Can't move that way\n");
                runAgain = 1;
            }
        }
    }
}

void moveBugs(Graph* maze) {
    bfs(maze, maze->tronIndex);
    NodeList* bug = maze->bugs;
    while (bug != NULL){
        GraphNode* bugNode = maze->graphArray[bug->nodeIndex];
        int newIndex = bugNode->bfsParentIndex;
        if (maze->graphArray[newIndex]->data == ' ') {
            maze->graphArray[newIndex]->data = bugNode->data;
            maze->graphArray[bug->nodeIndex]->data = ' ';
            bug->nodeIndex = newIndex;
        } else if (maze->graphArray[newIndex]->data == 'T'){
            printf("Bruh moment\n");
            printMaze(maze);
            exit(0);
        }
        bug = bug->next;
    }
    printMaze(maze);
}

void bfs(Graph* graph, int startingNodeIndex){
    Queue* queue = malloc(sizeof(Queue));
    queue->queue = NULL;
    queue->arraySize = graph->rowLength * graph->colHeight;
    enqueue(queue, startingNodeIndex);
    while (!queueIsEmpty(queue)){
        int index = dequeue(queue);
        GraphNode* vertex = graph->graphArray[index];
        if(vertex->label != VISITED){
            vertex->label = VISITED;

            NodeList* neighbor = vertex->adjNodes;
            while (neighbor != NULL){
                int label = graph->graphArray[neighbor->nodeIndex]->label;
                if (label != VISITED && label != NOT_VISITED){
                    GraphNode* neighborNode = graph->graphArray[neighbor->nodeIndex];
                    neighborNode->label = NOT_VISITED;
                    neighborNode->bfsParentIndex = index;
                    enqueue(queue, neighbor->nodeIndex);
                }
                neighbor = neighbor->next;
            }
        }
    }
}

void enqueue(Queue* queue, int data) {
    if(queue->queue == NULL){
        queue->queue = malloc(sizeof(int) * queue->arraySize);
        for (int i = 0; i < queue->arraySize; ++i) {
            queue->queue[i] = -1;
        }
        queue->front = 0;
        queue->back = 0;
    }
    if(queue->queue[queue->back % queue->arraySize] == -1){
        queue->queue[queue->back % queue->arraySize] = data;
        queue->back++;
    }
}

int dequeue(Queue* queue){
    int data = queue->queue[queue->front % queue->arraySize];
    queue->queue[queue->front % queue->arraySize] = -1;
    queue->front++;
    return data;
}

void printMaze(Graph* maze) {
    printf(" ");
    for (int j = 0; j < maze->rowLength; ++j) {
        printf("%d", j);
    }
    printf("\n");

    int lineCounter = 0;
    for (int i = 0; i < maze->rowLength * maze->colHeight; ++i) {
        if(i % maze->rowLength == 0){
            printf("%d", lineCounter);
            lineCounter++;
        }
        printf("%c", maze->graphArray[i]->data);
        // Add a newline after the last char in  a row
        if(i % maze->rowLength == maze->rowLength - 1){
            printf("\n");
        }
    }
}

void readMazeFromFile(char* filename, Graph* graph){
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

void addGraphNode(char data, Graph* graph) {
    GraphNode* newNode = malloc(sizeof(GraphNode));
    newNode->data = data;
    newNode->adjNodes = NULL;
    newNode->label = NOT_ON_QUEUE;
    graph->graphArray[graph->emptyIndex] = newNode;

    if (data == 'T'){
       graph->tronIndex = graph->emptyIndex;
    } 
    // If it's a bug
    else if (data != ' ' && data != '#' && data != 'I'){
        addNodeToList(&graph->bugs, graph->emptyIndex);
    }

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
//        addNodeToList(graph->graphArray[graph->emptyIndex], graph->emptyIndex - graph->rowLength);
        addNodeToList(&graph->graphArray[graph->emptyIndex]->adjNodes, up(graph->emptyIndex, graph->rowLength));
        addNodeToList(&graph->graphArray[up(graph->emptyIndex, graph->rowLength)]->adjNodes, graph->emptyIndex);
    }

    // If the node isn't at the start of a row
    if(graph->emptyIndex % graph->rowLength != 0){
        // Make this node and the one before it adjacent to each other
        addNodeToList(&graph->graphArray[graph->emptyIndex]->adjNodes, graph->emptyIndex - 1);
        addNodeToList(&graph->graphArray[graph->emptyIndex - 1]->adjNodes, graph->emptyIndex);
    }
    graph->emptyIndex++;
}

//TODO: This maybe can be reworked into having node->adjNodes be an array of size 4
void addNodeToList(NodeList** list, int nodeIndex) {
    NodeList* newNode = malloc(sizeof(NodeList));
    newNode->nodeIndex = nodeIndex;

    if(*list == NULL){
        *list = newNode;
    } else {
        if((*list)->nodeIndex > nodeIndex){
            newNode->next = *list;
            *list = newNode;
        } else {
            //This puts the indices in order by sorting since up < left < right < down
            NodeList* prevNode = *list;
            NodeList* tempNode = (*list)->next;
            while (tempNode != NULL && tempNode->nodeIndex < nodeIndex){
                prevNode = tempNode;
                tempNode = tempNode->next;
            }
            newNode->next = tempNode;
            prevNode->next = newNode;
        }
    }
}
