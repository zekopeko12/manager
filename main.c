#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct Subject{
    uint16_t grade;
    char* name;
} SUBJECT;

typedef struct Student{
    SUBJECT* subjects[6];
    int age;
    char* name;
    bool passed;
} STUDENT;

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
    STUDENT* students[6];
    
    for(int i=0; i<6; i++){
        students[i] = (STUDENT*)malloc(sizeof(STUDENT));
    
        students[i]->age = 19;
        students[i]->name = (char*)malloc(14);
        strcpy(students[i]->name, "Antonio Zeko");
        students[i]->passed = true;

        for(int j=0; j<6; j++){
            students[i]->subjects[j] = (SUBJECT*)malloc(sizeof(SUBJECT));
            students[i]->subjects[j]->grade = 5;

            students[i]->subjects[j]->name = (char*)malloc(8);
            strcpy(students[i]->subjects[j]->name, "predmet");
        }
    }

    FILE* db = fopen("data.db", "wb");

    fwrite(students, sizeof(STUDENT), 6, db);
    fclose(db);    

    db = fopen("data.db", "rb");

    STUDENT student[6];
    fread(student, sizeof(STUDENT), 6, db);

    printf("%d", &student[0].age);
}