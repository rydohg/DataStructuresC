/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:

*/
#include "hw6.h"
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
void printParents(Graph *maze);

// Position helpers
int up(int index, int rowLength) { return index - rowLength; }
int down(int index, int rowLength) { return index + rowLength; }
int left(int index, int rowLength) { return (index % rowLength != 0) ? index - 1 : -1; }
int right(int index, int rowLength) { return (index % rowLength != rowLength - 1) ? index + 1 : -1; }

// BFS constants
const int NOT_ON_QUEUE = 0;
const int NOT_VISITED_ON_QUEUE = 1;
const int VISITED = 2;

// These are the indices used by addGraphNode to add to the adjNodes array for each node
/* TODO: BFS will go in order 0-3 and the HW6.pdf says BFS should go up, down, left, right but
 * the only way to reproduce testOutput1.txt is to go right, down, left, up
 */
const int UP = 3;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 0;

int main(int argc, char **args) {
    if (argc < 2) {
        printf("No input\n");
        exit(1);
    }
    // Initialize the maze
    Graph maze;
    maze.graphArray = NULL;
    maze.bugs = NULL;
    maze.emptyIndex = 0;

    readMazeFromFile(args[1], &maze);
    // Turn the graph's array into a traditional array so it can be represented in CLion's debugger
    GraphNode *debugArray[maze.rowLength * maze.colHeight];
    for (int i = 0; i < maze.rowLength * maze.colHeight; ++i) {
        debugArray[i] = maze.graphArray[i];
    }

    // Game loop. This loop is terminated by either a win or a loss
    while (1){
        printMaze(&maze);
        readInput(&maze);
        printMaze(&maze);
        moveBugs(&maze);
    }
}

void readInput(Graph *graph) {
    // Keep asking for input until we get a valid move or if Tron can't move
    int runAgain = 1;
    while (runAgain) {
        runAgain = 0;
        char *move = malloc(sizeof(char) * 2);
        printf("Please enter your move [u(p), d(own), l(elf), or r(ight)]: ");
        // Gets move character, newline, and \0
        fgets(move, 3, stdin);

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
        if (graph->graphArray[newIndex]->data == ' ') {
            graph->graphArray[graph->tronIndex]->data = ' ';
            graph->graphArray[newIndex]->data = 'T';
            graph->graphArray[newIndex]->bfsParentIndex = -1;
            graph->tronIndex = newIndex;
        }
            // Exit the program if Tron wins
        else if (graph->graphArray[newIndex]->data == 'I') {
            printMaze(graph);
            printf("Tron reaches I/O Tower\n");
            exit(0);
        }
            // If Tron can move then keep asking for input
        else if (graph->graphArray[up(graph->tronIndex, graph->rowLength)]->data == ' ' ||
                 graph->graphArray[down(graph->tronIndex, graph->rowLength)]->data == ' ' ||
                 graph->graphArray[left(graph->tronIndex, graph->rowLength)]->data == ' ' ||
                 graph->graphArray[right(graph->tronIndex, graph->rowLength)]->data == ' ') {
            printf("Can't move that way\n");
            runAgain = 1;
        }
        // If Tron can't move any way then the loop ends and then we make the bugs move which would kill Tron
    }
}

void moveBugs(Graph *maze) {
    // For each bug determine the next move as calculated in BFS then create the output specified in HW6.pdf
    NodeList *bug = maze->bugs;
    while (bug != NULL) {
        // Reset the labels and parentIndices between each run of BFS
        for (int i = 0; i < maze->rowLength * maze->colHeight; ++i) {
            maze->graphArray[i]->label = NOT_ON_QUEUE;
            maze->graphArray[i]->bfsParentIndex = -1;
        }
        // Run BFS to create a spanning tree made from each node
        // storing the index of its parent in the spanning tree
        // Following the parent indices is the shortest path to Tron
        bfs(maze, maze->tronIndex);

//        printParents(maze);
        GraphNode *bugNode = maze->graphArray[bug->nodeIndex];
        int newIndex = bugNode->bfsParentIndex;
        // Don't try to go to a non-existent node. This happens if we can't reach Tron from this bug
        if(newIndex < 0){
            bug = bug->next;
            continue;
        }
        if (maze->graphArray[newIndex]->data == ' ') {
            // Determine which direction BFS is taking us
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
            int nextVertex = bug->nodeIndex;
            while (maze->graphArray[nextVertex]->bfsParentIndex != -1) {
                shortestLength++;
                nextVertex = maze->graphArray[nextVertex]->bfsParentIndex;
            }
            printf(" %d", shortestLength);

            // Reset nextVertex and print the nodes on the shortest path including Tron's index
            // The while condition is different because the while loop above ignore's Tron's index
            // whose bfsParentIndex is -1 but this runs one more time by having it check nextVertex
            // instead of the current vertex
            nextVertex = bug->nodeIndex;
            while (nextVertex != -1) {
                printf(" (%d,%d)", nextVertex / maze->rowLength, nextVertex % maze->rowLength);
                nextVertex = maze->graphArray[nextVertex]->bfsParentIndex;
            }
            printf("\n");

            // Actually move the bug
            maze->graphArray[newIndex]->data = bugNode->data;
            maze->graphArray[bug->nodeIndex]->data = ' ';
            bug->nodeIndex = newIndex;
        }
            // If the bug reaches Tron then output that Tron lost and exit
        else if (maze->graphArray[newIndex]->data == 'T') {
            printMaze(maze);
            printf("A bug is not hungry any more!\n");
            exit(0);
        }
        bug = bug->next;
    }
}

void bfs(Graph *graph, int startingNodeIndex) {
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
            for (int i = 0; i < 4; ++i) {
                GraphNode *neighborNode = graph->graphArray[neighbors[i]];
                int label = neighborNode->label;
                // The last clause in the if statement handles one of the conditions that can cause a collision between bugs
                // Don't add to the spanning tree if the parent and its neighbor are bugs meaning don't put another bug
                // as the next move in the shortest path for a bug
                if (label != VISITED &&
                    label != NOT_VISITED_ON_QUEUE &&
                    neighborNode->data != '#' &&
                    neighborNode->data != 'I' &&
                    !((vertex->data >= 'a' && vertex->data <= 'z') && (neighborNode->data >= 'a' && neighborNode->data <= 'z'))) {
                    neighborNode->label = NOT_VISITED_ON_QUEUE;
                    // Each node has an int that points to that node's parent
                    // in the BFS spanning tree, allowing us to find the shortest path
                    neighborNode->bfsParentIndex = index;
                    enqueue(queue, neighbors[i]);
                }
            }
        }
    }
}

void enqueue(Queue *queue, int data) {
    // Queue made using a circular array
    // Initialize the queue, this assumes that arraySize is correctly initialized
    if (queue->queue == NULL) {
        queue->queue = malloc(sizeof(int) * queue->arraySize);
        for (int i = 0; i < queue->arraySize; ++i) {
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

int dequeue(Queue *queue) {
    int data = queue->queue[queue->front % queue->arraySize];
    queue->queue[queue->front % queue->arraySize] = -1;
    queue->front++;
    return data;
}

void printMaze(Graph *maze) {
    printf(" ");
    for (int j = 0; j < maze->rowLength; ++j) {
        printf("%d", j);
    }
    printf("\n");

    int lineCounter = 0;
    for (int i = 0; i < maze->rowLength * maze->colHeight; ++i) {
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

void printParents(Graph *maze) {
    printf("--------------\n");
    for (int i = 0; i < maze->rowLength * maze->colHeight; ++i) {
        printf("%02d:%02d ", i, maze->graphArray[i]->bfsParentIndex);
        // Add a newline after the last char in  a row
        if (i % maze->rowLength == maze->rowLength - 1) {
            printf("\n");
        }
    }
    printf("--------------\n");
}

void readMazeFromFile(char *filename, Graph *graph) {
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
    GraphNode *newNode = malloc(sizeof(GraphNode));
    newNode->data = data;
    newNode->label = NOT_ON_QUEUE;
    graph->graphArray[graph->emptyIndex] = newNode;

    // Keep track of Tron's index to avoid searching for it
    if (data == 'T') {
        graph->tronIndex = graph->emptyIndex;
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
void addNodeToList(NodeList **list, int nodeIndex, Graph* graph) {
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