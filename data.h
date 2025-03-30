
#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Subject{
    uint16_t grade;
    char name[60];
} SUBJECT;

typedef struct Student{
    SUBJECT** subjects;
    int age;
    char name[20];
    bool passed;
} STUDENT;

void readFromFile(STUDENT* students, int count);
void writeToFile(int count);

#endif