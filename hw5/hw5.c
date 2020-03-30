/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:
*/
#include "hw5.h"

int main(int argc, char** args){
    if(argc < 2){
        printf("No input file given\n");
        exit(1);
    }
//    readInput(args[1]);

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