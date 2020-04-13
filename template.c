/*
  Author: Ryan Doherty
  Email: rdoherty2019@my.fit.edu
  Course: CSE2010
  Section: 14

  Description of this file:

*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** args){

    return 0;
}

void readInput(char* filename){
    FILE *inputFile = fopen(filename, "r");

    if(inputFile == NULL){
        printf("Couldn't open file\n");
        exit(1);
    }
    const int LINE_SIZE = 32;
    // For each line in the given file extract and run the given command
    char *currentLine = malloc(LINE_SIZE * sizeof(char));
    while (fgets(currentLine, LINE_SIZE - 1, inputFile) != NULL) {
        // Remove newlines from the end of the line.
        // This removes the first instance of \n or \r
        currentLine[strcspn(currentLine, "\n\r")] = 0;
        // Make a string of all the chars before the first space
        // Aka. get the first word in a line
        char *token = strtok(currentLine, " ");
        printf("%s", token);


        printf("\n");
        lineCounter++;
    }
    fclose(inputFile);
    free(currentLine);
}