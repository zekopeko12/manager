#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Subject{
    int grade;
    char name[60];
} SUBJECT;

typedef struct Student{
    SUBJECT** subjects;
    int age;
    char name[50];
    bool passed;
} STUDENT;

void readFromFile(int count);
void writeToFile(int count);

#endif