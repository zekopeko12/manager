#include "data.h"

void writeToFile(int size){

    STUDENT* students[size];
    int numOfSubjects = 0;

    for(int i=0; i<size; i++){
        students[i] = (STUDENT*)malloc(sizeof(STUDENT));
    }

    for(int i=0; i<size; i++){
        printf("\nName of student: ");
        scanf("%s", students[i]->name);
        printf("\nAge of student: ");
        scanf("%d", &students[i]->age);
        printf("\nHow many subjects is the student attending? ");
        scanf("%d", &numOfSubjects);

        students[i]->subjects = (SUBJECT**)malloc(numOfSubjects * sizeof(SUBJECT*));

        for(int j=0; j<numOfSubjects; j++){
            students[i]->subjects[j] = (SUBJECT*)malloc(sizeof(SUBJECT));
            printf("\nName of subject: ");
            scanf("%s", students[i]->subjects[j]->name);
            printf("\nWhat grade did the student get? ");
            scanf("%d", &students[i]->subjects[j]->grade);
        }

        students[i]->subjects = false;
    }

    FILE *db = fopen("data.db", "wb");

    for(int i=0; i<size; i++){
        fwrite(&students[i]->age, sizeof(int), 1, db);
        fwrite(&students[i]->passed, sizeof(bool), 1, db);
        fwrite(students[i]->name, sizeof(char), 50, db);

        fwrite(&numOfSubjects, sizeof(int), 1, db);

        for(int j=0; j<numOfSubjects; j++){
            fwrite(&students[i]->subjects[j]->grade, sizeof(int), 1, db);
            fwrite(students[j]->subjects[j]->name, sizeof(char), 60, db);
        }
    }

    fclose(db);

    for (int i=0; i<size; i++) {
        for (int j=0; j<numOfSubjects; j++) {
            free(students[i]->subjects[j]);
        }
        free(students[i]->subjects);
        free(students[i]);
    }
}
