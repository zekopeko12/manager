#include "data.h"
#include <string.h>

void printStudents(STUDENT *students[], int count){
    printf("----\n");
    for(int i=0; i<count; i++){
        printf("%s \t %d \t %s\n", students[i]->name, students[i]->age, students[i]->passed ? "true" : "false");

        for(int j=0; j < 6; j++){
            printf("Subject: %s\n", students[i]->subjects[j]->name);
            printf("Grade: %d\n", students[i]->subjects[j]->grade);
        }

        printf("----\n");
    }
}

int main(int argc, char *argv[]) {
    int choice;

    STUDENT students[6];

    while(true){
        printf("\nHello!");
        printf("\nChoose what you would like to do:");
        printf("\n1. List the students");
        printf("\n2. Input Students");
        printf("\n3. Exit");

        switch(choice){
            case 1:
                readFromFile(students, 6);
                break;
            case 2:
                printf("\nHow many would you like to input?");
                int count = 0;
                scanf("%d", &count);
                writeToFile(count);
                break;
            case 3:
                exit(EXIT_SUCCESS);
        }
    }
}