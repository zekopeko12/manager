#include "data.h"

void writeToFile(int size){

    STUDENT* students[size];
    int numOfSubjects = 0;

    for(int i=0; i<size; i++){
        students[i] = (STUDENT*)malloc(sizeof(STUDENT));
    }

    for(int i=0; i<size; i++){
        printf("\nName of student: ");
        scanf("%19[^n]", students[i]->name);
        printf("\nAge of student: ");
        scanf("%d", students[i]->age);
        printf("\nHow many subjects is the student attending?");
        scanf("%d", &numOfSubjects);
        for(int j=0; j<numOfSubjects; j++){
            students[i]->subjects[j] = (SUBJECT*)malloc(sizeof(SUBJECT));
            printf("\nName of subject: ");
            scanf("%59[^n]", students[i]->subjects[j]->name);
            printf("\nWhat grade did the student get? ");
            scanf("%d", students[i]->subjects[j]->grade);
        }
    }

    FILE *db = fopen("data.db", "wb");
    fwrite(students, sizeof(STUDENT), size, db);
    fclose(db);
}

void readFromFile(STUDENT *students, int size){
    FILE *db = fopen("data.db", "rb");
    fread(students, sizeof(STUDENT), size, db);
    fclose(db);
}
