/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:
  HW6.c is a very simplified version of the I/O tower game from Tron.
  Where the player Tron represented as the character 'T' tries to
  reach the I/O tower 'I' while avoiding the bugs trying to kill him represented
  as letters 'a' through 'z'.
  It takes a maze as input from a file given as a program argument
  that contains a game world and the starting positions of Tron and the bugs
  then asks the player to input which direction to move to hopefully get to the I/O tower.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

// Linked list to store a list of the bugs
typedef struct nodeList {
    int nodeIndex;
    struct nodeList* next;
} NodeList;

// An adjacency list that uses an array since each vertex can only have 4 adjacent nodes
typedef struct node {
    char data;
    int adjNodes[4];
    int label;
    int bfsParentIndex;
    int onTronsPath;
    int dijkstraLabel;
    int dijkstraParentIndex;
    int inQueue;
} GraphNode;

// Data struct that stores the game world and its variables
typedef struct {
    GraphNode** graphArray;
    int rowLength, colHeight;
    int emptyIndex;
    int tronIndex;
    int towerIndex;
    NodeList* bugs;
} Graph;

typedef struct {
    int* queue;
    int front, back, arraySize, queueSize;
} Queue;

/*typedef struct {
    int label;
    GraphNode* node;
} Item;*/

typedef struct {
    int** heap;
    int size;
    int emptyIndex;
} PriorityQueue;

void readMazeFromFile(char *filename, Graph *graph);
void addGraphNode(char data, Graph *graph);
void readInput(Graph *graph);
void moveBugs(Graph *maze);
void bfs(Graph *graph, int startingNodeIndex);
void printMaze(Graph *maze);
void addNodeToList(NodeList **list, int nodeIndex, Graph* graph);
void enqueue(Queue *queue, int data);
int dequeue(Queue *queue);
int queueIsEmpty(Queue *queue) { return (queue->queue[queue->front % queue->arraySize] != -1) ? 0 : 1; }

// Position helpers
int up(int index, int rowLength) { return index - rowLength; }
int down(int index, int rowLength) { return index + rowLength; }
int left(int index, int rowLength) { return (index % rowLength != 0) ? index - 1 : -1; }
int right(int index, int rowLength) { return (index % rowLength != rowLength - 1) ? index + 1 : -1; }

// Extra Credit 2
int heapParent(int index) { return (index - 1)/2; }
int heapLeft(int index) { return 2*index + 1; }
int heapRight(int index) { return 2*index + 2; }
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Dijkstra's stuff
void addToPQueue(int data, PriorityQueue* queue, Graph* graph);
int removeMin(PriorityQueue* pQueue, Graph* graph);
void minHeapify(int index, PriorityQueue* pQueue, Graph* graph);
void dijkstra(int startIndex, Graph* graph);

// BFS constants
const int NOT_ON_QUEUE = 0;
const int NOT_VISITED_ON_QUEUE = 1;
const int VISITED = 2;

// These are the indices used by addGraphNode to add to the adjNodes array for each node
// They go in the opposite order of up, down, left, right because this is the order they are enqueued.
// They are taken off the queue and visited in the order up, down, left, right.
const int UP = 3;
const int DOWN = 2;
const int LEFT = 1;
const int RIGHT = 0;

int main(int argc, char **args) {
    if (argc < 2) {
        printf("No input\n");
        exit(1);
    }
/*    PriorityQueue* pQueue = malloc(sizeof(PriorityQueue));
    pQueue->size = 2;
    GraphNode* testNode = malloc(sizeof(GraphNode));
    testNode->data = 'T';
    Item* item = malloc(sizeof(Item));
    item->node = testNode;
    item->label = 1;
    addToPQueue(item, pQueue);
    printf("idk");*/
    // Initialize the maze
    Graph maze;
    maze.graphArray = NULL;
    maze.bugs = NULL;
    maze.emptyIndex = 0;

    readMazeFromFile(args[1], &maze);
    dijkstra(maze.tronIndex, &maze);
    GraphNode* debugArray[maze.rowLength * maze.colHeight];
    int i;
    for (i = 0; i < maze.rowLength * maze.colHeight; ++i) {
        debugArray[i] = maze.graphArray[i];
    }
    printf("idk");
//    while (1){
/*        printMaze(&maze);
        readInput(&maze);
        printMaze(&maze);
        moveBugs(&maze);*/
//    }
}

int findInPQueue(int nodeIndex, PriorityQueue* pQueue){
    for (int i = 0; i < pQueue->emptyIndex; ++i) {
        if (*pQueue->heap[i] == nodeIndex){
            return i;
        }
    }
    return -1;
}

void dijkstra(int startIndex, Graph* graph){
    // Initialize all values of D[] to infinity except the start
    int graphSize = graph->rowLength * graph->colHeight;
    int distancesArray[graphSize];/* = malloc(sizeof(int) * graphSize);*/
    for (int i = 0; i < graphSize; ++i) {
        distancesArray[i] = INT_MAX;
    }
    distancesArray[startIndex] = 0;

    // Initialize Priority Queue
    PriorityQueue* pQueue = malloc(sizeof(PriorityQueue));
    pQueue->heap = NULL;
    pQueue->size = graphSize;

    for (int j = 0; j < graphSize; ++j) {
        graph->graphArray[j]->dijkstraLabel = distancesArray[j];
        graph->graphArray[j]->inQueue = 1;
        addToPQueue(j, pQueue, graph);
    }

    int debugArray[pQueue->size];
    for (int j = 0; j < pQueue->size; ++j) {
        debugArray[j] = *pQueue->heap[j];
    }

    // Algorithm
    while (pQueue->emptyIndex > 0){
        int minIndex = removeMin(pQueue, graph);
        GraphNode* min = graph->graphArray[minIndex];
        min->inQueue = 0;

        if(min->data == '#' || min->data == 'I'){
            continue;
        }

        for (int i = 0; i < 4; ++i) {
            int neighborIndex = min->adjNodes[i];
            GraphNode* neighbor = graph->graphArray[neighborIndex];
            if(neighbor->inQueue && neighbor->data != '#' && neighbor->data != 'I'){
                // Set distance between these vertices based on what hw6.pdf says
                int distBetween;
                if(min->onTronsPath && neighbor->onTronsPath){
                    distBetween = 1;
                } else if(min->onTronsPath && neighbor->onTronsPath){
                    distBetween = 2;
                } else {
                    distBetween = 3;
                }
                // Edge Relaxation
                if(distancesArray[minIndex] + distBetween < distancesArray[neighborIndex]){
                    // Update distance
                    distancesArray[neighborIndex] = distancesArray[minIndex] + distBetween;

                    // Update Priority Queue
                    int heapIndex = findInPQueue(neighborIndex, pQueue);
                    graph->graphArray[*pQueue->heap[heapIndex]]->dijkstraLabel = distancesArray[minIndex] + distBetween;
                    int index = heapIndex;
                    while (index > 0 &&
                        graph->graphArray[*pQueue->heap[heapParent(index)]]->dijkstraLabel > graph->graphArray[*pQueue->heap[index]]->dijkstraLabel){
                        swap(pQueue->heap[index], pQueue->heap[heapParent(index)]);
                        index = heapParent(index);
                    }

                    // Set the vertex's parent to the new shortest path
                    neighbor->dijkstraParentIndex = minIndex;
                }
            }
        }
    }
    printf("idk");
/*    for (int j = 0; j < pQueue->size; ++j) {
        debugArray[j] = pQueue->heap[j];
    }*/
}

void readInput(Graph* graph) {
    // Keep asking for input until we get a valid move or if Tron can't move
    int runAgain = 1;
    while (runAgain) {
        runAgain = 0;
        char *move = malloc(sizeof(char) * 3);
        printf("Please enter your move [u(p), d(own), l(elf), or r(ight)]: ");
        // Gets move character, newline, and \0
        fgets(move, 2, stdin);
        // Flush stdin so we don't run multiple times if someone enters more than one character
        while (getchar() != '\n');

        // newIndex is where the player wants Tron to go and we set it using this switch statement
        // We set runAgain to be true if the move isn't u, d, l, or r
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
                runAgain = 1;
                break;
        }
        // Don't attempt to move if we don't have a valid direction
        if(runAgain) { continue; }

        // If we can move to a cell then do it
        if (graph->graphArray[newIndex]->data == ' ' || graph->graphArray[newIndex]->data == 'I') {
            char newIndexOldData = graph->graphArray[newIndex]->data;

            graph->graphArray[graph->tronIndex]->data = ' ';
            graph->graphArray[newIndex]->data = 'T';
            graph->graphArray[newIndex]->bfsParentIndex = -1;
            graph->tronIndex = newIndex;

            // Exit the program if Tron wins
            if(newIndexOldData == 'I'){
                printMaze(graph);
                printf("Tron reaches I/O Tower\n");
                exit(0);
            }
        }
            // If Tron can move then keep asking for input
        else if (graph->graphArray[up(graph->tronIndex, graph->rowLength)]->data == ' ' ||
                 graph->graphArray[down(graph->tronIndex, graph->rowLength)]->data == ' ' ||
                 graph->graphArray[left(graph->tronIndex, graph->rowLength)]->data == ' ' ||
                 graph->graphArray[right(graph->tronIndex, graph->rowLength)]->data == ' ') {
            runAgain = 1;
        }
        // If Tron can't move any way then the loop ends and then we make the bugs move which would kill Tron
        free(move);
    }
}

void moveBugs(Graph* maze) {
    // Reset the labels and parentIndices between each run of BFS
    int i;
    for (i = 0; i < maze->rowLength * maze->colHeight; ++i) {
        maze->graphArray[i]->label = NOT_ON_QUEUE;
        maze->graphArray[i]->bfsParentIndex = -1;
    }
    // Find Tron's shortest path
    bfs(maze, maze->towerIndex);

    // Set the nodes on the path to onTronsPath
    int nextVertex = maze->tronIndex;
    while (nextVertex != -1){
        maze->graphArray[nextVertex]->onTronsPath = 1;
        nextVertex = maze->graphArray[nextVertex]->bfsParentIndex;
    }

    // For each bug determine the next move as calculated in BFS then create the output specified in HW6.pdf
    NodeList *bug = maze->bugs;
    while (bug != NULL) {
        // Reset the labels and parentIndices between each run of BFS
        int j;
        for (j = 0; j < maze->rowLength * maze->colHeight; ++j) {
            maze->graphArray[j]->label = NOT_ON_QUEUE;
            maze->graphArray[j]->bfsParentIndex = -1;
        }
        // Run Dijkstra's to find the best path to get to Tron
        dijkstra(maze->tronIndex, maze);

        GraphNode *bugNode = maze->graphArray[bug->nodeIndex];
        int newIndex = bugNode->dijkstraParentIndex;
        // If the bug doesn't have a path to Tron then just print the bug name
        if(newIndex < 0){
            printf("Bug %c:\n", bugNode->data);
            bug = bug->next;
            continue;
        } else if (maze->graphArray[newIndex]->data == ' ' || maze->graphArray[newIndex]->data == 'T') {
            // Determine which direction Dijkstra's is taking us
            char move = ' ';
            if (newIndex < bug->nodeIndex) {
                if (newIndex == bug->nodeIndex - 1) {
                    move = 'l';
                } else {
                    move = 'u';
                }
            } else {
                if (newIndex == bug->nodeIndex + 1) {
                    move = 'r';
                } else {
                    move = 'd';
                }
            }

            printf("Bug %c: %c", bugNode->data, move);

            // Find the length of the shortest path and print it out
            int shortestLength = 0;
            nextVertex = bug->nodeIndex;
            while (maze->graphArray[nextVertex]->dijkstraParentIndex != -1) {
                shortestLength++;
                nextVertex = maze->graphArray[nextVertex]->dijkstraParentIndex;
            }
            printf(" %d", shortestLength);

            // Reset nextVertex and print the nodes on the shortest path including Tron's index
            // The while condition is different because the while loop above ignore's Tron's index
            // whose bfsParentIndex is -1 but this runs one more time by having it check nextVertex
            // instead of the current vertex
            nextVertex = bug->nodeIndex;
            while (nextVertex != -1) {
                printf(" (%d,%d)", nextVertex / maze->rowLength, nextVertex % maze->rowLength);
                nextVertex = maze->graphArray[nextVertex]->dijkstraParentIndex;
            }
            printf("\n");

            char newSpaceOldData = maze->graphArray[newIndex]->data;
            // Actually move the bug
            maze->graphArray[newIndex]->data = bugNode->data;
            maze->graphArray[bug->nodeIndex]->data = ' ';
            bug->nodeIndex = newIndex;

            // If the bug reaches Tron then output that Tron lost and exit
            if (newSpaceOldData == 'T'){
                printMaze(maze);
                printf("A bug is not hungry any more!\n");
                exit(0);
            }
        }
        bug = bug->next;
    }
}

void bfs(Graph* graph, int startingNodeIndex) {
    // Create and initialize the queue
    Queue *queue = malloc(sizeof(Queue));
    queue->queue = NULL;
    queue->arraySize = graph->rowLength * graph->colHeight;

    // Implementation of the BFS pseudocode from the notes
    enqueue(queue, startingNodeIndex);
    while (!queueIsEmpty(queue)) {
        int index = dequeue(queue);
        GraphNode *vertex = graph->graphArray[index];
        if (vertex->label != VISITED) {
            vertex->label = VISITED;

            int *neighbors = vertex->adjNodes;
            int i;
            for (i = 0; i < 4; ++i) {
                GraphNode *neighborNode = graph->graphArray[neighbors[i]];
                int label = neighborNode->label;

                // The last clause in the if statement makes BFS treat other bugs as obstacles
                // Don't create any paths that have another bug on it
                if (label != VISITED &&
                    label != NOT_VISITED_ON_QUEUE &&
                    neighborNode->data != '#' &&
                    neighborNode->data != 'I' &&
                    !(vertex->data >= 'a' && vertex->data <= 'z')) {
                    neighborNode->label = NOT_VISITED_ON_QUEUE;
                    // Each node has an int that points to that node's parent
                    // in the BFS spanning tree, allowing us to find the shortest path
                    neighborNode->bfsParentIndex = index;
                    enqueue(queue, neighbors[i]);
                }
            }
        }
    }
    free(queue->queue);
    free(queue);
}

void enqueue(Queue* queue, int data) {
    // Queue made using a circular array
    // Initialize the queue, this assumes that arraySize is correctly initialized
    if (queue->queue == NULL) {
        queue->queue = malloc(sizeof(int) * queue->arraySize);
        int i;
        for (i = 0; i < queue->arraySize; ++i) {
            queue->queue[i] = -1;
        }
        queue->front = 0;
        queue->back = 0;
    }
    // If there's room in the queue then add the value to it
    if (queue->queue[queue->back % queue->arraySize] == -1) {
        queue->queue[queue->back % queue->arraySize] = data;
        queue->back++;
    }
}

int dequeue(Queue* queue) {
    int data = queue->queue[queue->front % queue->arraySize];
    queue->queue[queue->front % queue->arraySize] = -1;
    queue->front++;
    return data;
}

void printMaze(Graph* maze) {
    // Print the column index row
    printf(" ");
    int j;
    for (j = 0; j < maze->rowLength; ++j) {
        printf("%d", j);
    }
    printf("\n");

    int lineCounter = 0;
    int i;
    for (i = 0; i < maze->rowLength * maze->colHeight; ++i) {
        // Print and keep track of the row number if this is the beginning of a row
        if (i % maze->rowLength == 0) {
            printf("%d", lineCounter);
            lineCounter++;
        }
        printf("%c", maze->graphArray[i]->data);
        // Add a newline after the last char in  a row
        if (i % maze->rowLength == maze->rowLength - 1) {
            printf("\n");
        }
    }
}

void readMazeFromFile(char* filename, Graph* graph) {
    FILE *inputFile = fopen(filename, "r");

    if (inputFile == NULL) {
        printf("Couldn't open file\n");
        exit(1);
    }

    int lineCounter = 0;
    const int LINE_SIZE = 32;
    char *currentLine = malloc(LINE_SIZE * sizeof(char) + 1);
    while (fgets(currentLine, LINE_SIZE, inputFile) != NULL) {
        // Remove newlines from the end of the line.
        // This cuts off the string at the first instance of \n or \r
        currentLine[strcspn(currentLine, "\n\r")] = 0;

        // The first line contains the dimensions for the graph
        if (lineCounter == 0) {
            char *mazeSizeString = strtok(currentLine, " ");
            graph->rowLength = atoi(mazeSizeString);
            graph->colHeight = atoi(strtok(NULL, " "));
            graph->graphArray = calloc(graph->rowLength * graph->colHeight, sizeof(GraphNode *));
        }
        // The rest of the lines contain the maze. Each character is added to the graph.
        else {
            int i;
            for (i = 0; i < strlen(currentLine); ++i) {
                addGraphNode(currentLine[i], graph);
            }
        }
        lineCounter++;
    }
    fclose(inputFile);
    free(currentLine);
}

void addGraphNode(char data, Graph* graph) {
    GraphNode *newNode = malloc(sizeof(GraphNode));
    newNode->data = data;
    newNode->label = NOT_ON_QUEUE;
    graph->graphArray[graph->emptyIndex] = newNode;

    // Keep track of Tron's index to avoid searching for it
    if (data == 'T') {
        graph->tronIndex = graph->emptyIndex;
    }
    // Keep track of IO tower's index
    else if(data == 'I'){
        graph->towerIndex = graph->emptyIndex;
    }
    // If it's a bug let's keep track of them using their indices in a list so we don't have to search for them later
    else if (data != ' ' && data != '#' && data != 'I') {
        addNodeToList(&graph->bugs, graph->emptyIndex, graph);
    }

    // Check if the input maze is bigger than the given dimensions
    if (graph->emptyIndex >= graph->rowLength * graph->colHeight) {
        printf("Input maze is wrong\n");
        exit(1);
    }

    // If the new node isn't in the first row then make this node and the one above it adjacent to each other
    if (graph->emptyIndex / graph->rowLength != 0) {
        graph->graphArray[graph->emptyIndex]->adjNodes[UP] = up(graph->emptyIndex, graph->rowLength);
        graph->graphArray[up(graph->emptyIndex, graph->rowLength)]->adjNodes[DOWN] = graph->emptyIndex;
    }

    // If the node isn't at the start of a row then make this node and the one before it adjacent to each other
    if (graph->emptyIndex % graph->rowLength != 0) {
        graph->graphArray[graph->emptyIndex]->adjNodes[LEFT] = left(graph->emptyIndex, graph->rowLength);
        graph->graphArray[left(graph->emptyIndex, graph->rowLength)]->adjNodes[RIGHT] = graph->emptyIndex;
    }
    graph->emptyIndex++;
}

// Makes a linked list that keeps track of the bugs
void addNodeToList(NodeList** list, int nodeIndex, Graph* graph) {
    NodeList *newNode = malloc(sizeof(NodeList));
    newNode->nodeIndex = nodeIndex;

    if (*list == NULL) {
        *list = newNode;
    } else {
        if (graph->graphArray[(*list)->nodeIndex]->data > graph->graphArray[nodeIndex]->data) {
            newNode->next = *list;
            *list = newNode;
        } else {
            NodeList *prevNode = *list;
            NodeList *tempNode = (*list)->next;
            while (tempNode != NULL && graph->graphArray[tempNode->nodeIndex]->data < graph->graphArray[nodeIndex]->data) {
                prevNode = tempNode;
                tempNode = tempNode->next;
            }
            newNode->next = tempNode;
            prevNode->next = newNode;
        }
    }
}

void addToPQueue(int dataIndex, PriorityQueue* queue, Graph* graph){
    int* data = malloc(sizeof(int));
    *data = dataIndex;

    if(queue->heap == NULL){
        queue->heap = malloc(queue->size * sizeof(int*));
        queue->heap[0] = data;
        queue->emptyIndex = 1;
    } else {
        queue->heap[queue->emptyIndex] = data;

        int index = queue->emptyIndex;
        while (index > 0 && graph->graphArray[*queue->heap[heapParent(index)]]->dijkstraLabel > graph->graphArray[*queue->heap[index]]->dijkstraLabel){
            swap(queue->heap[index], queue->heap[heapParent(index)]);
            index = heapParent(index);
        }
        queue->emptyIndex++;
    }
}

void minHeapify(int index, PriorityQueue* pQueue, Graph* graph){
    int smallest = index;
    if (heapLeft(index) < pQueue->emptyIndex &&
        graph->graphArray[*pQueue->heap[heapLeft(index)]]->label < graph->graphArray[*pQueue->heap[index]]->label){
        smallest = heapLeft(index);
    }
    if (heapRight(index) < pQueue->emptyIndex &&
        graph->graphArray[*pQueue->heap[heapRight(index)]]->label < graph->graphArray[*pQueue->heap[smallest]]->label){
        smallest = heapRight(index);
    }
    if (smallest != index){
        swap(pQueue->heap[index], pQueue->heap[smallest]);
        minHeapify(smallest, pQueue, graph);
    }
}

int removeMin(PriorityQueue* pQueue, Graph* graph){
    int min = *pQueue->heap[0];
    pQueue->heap[0] = pQueue->heap[pQueue->emptyIndex-1];
    pQueue->heap[pQueue->emptyIndex - 1] = NULL;
    pQueue->emptyIndex--;

    minHeapify(0, pQueue, graph);
    return min;
}