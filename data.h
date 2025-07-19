#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PAGE_SIZE                       4096
#define MAX_PAGES                       100
#define COLUMN_SIZE                     32
#define MAX_SEMESTERS                   10
#define ROW_SIZE                        (STUDENT_FIXED_SIZE + MAX_SEMESTERS * sizeof(SEMESTER))
#define ROWS_PER_PAGE                   (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS                  (ROWS_PER_PAGE * MAX_PAGES)

#define SUBJECTS_PER_SEMESTER           6
#define SUBJECT_NAME_SIZE               60

#define STUDENT_AGE_SIZE                sizeof(int)
#define STUDENT_SEMESTER_COUNT_SIZE     sizeof(int)
#define STUDENT_ID_SIZE                 sizeof(int)
#define STUDENT_NAME_SIZE               60

#define STUDENT_AGE_OFFSET              0
#define STUDENT_SEMESTER_COUNT_OFFSET   (STUDENT_AGE_OFFSET + STUDENT_AGE_SIZE)
#define STUDENT_NAME_OFFSET             (STUDENT_SEMESTER_COUNT_OFFSET + STUDENT_SEMESTER_COUNT_SIZE)
#define STUDENT_ID_OFFSET               (STUDENT_NAME_OFFSET + STUDENT_NAME_SIZE)
#define STUDENT_FIXED_SIZE              (STUDENT_ID_OFFSET + STUDENT_ID_SIZE)

typedef struct Subject{
    char name[SUBJECT_NAME_SIZE];
    int grade;
} SUBJECT;

typedef struct Semester{
    SUBJECT subjects[SUBJECTS_PER_SEMESTER];
    bool passed;
}SEMESTER;

typedef struct Student{
    uint32_t id;
    uint32_t age;
    uint32_t semester_count;
    char name[STUDENT_NAME_SIZE];
    SEMESTER* semesters;
} STUDENT;

typedef struct Pager{
    int file;
    int file_length;
    int num_pages;
    void* pages[MAX_PAGES];
}PAGER;

typedef struct{
    PAGER* pager;
    int num_rows;
}TABLE;

PAGER* open_pager(const char* filename);
void serialize_row(STUDENT* source, void* destination);
void deserialize_row(void* source, STUDENT* destination);
int get_row_page(int row_num);
int row_offset(int row_num);
void* get_page(PAGER* pager, int page_num);
void flush_page(PAGER* pager, int page_num, int size);
void close_pager(PAGER* pager);
TABLE* db_open(const char* filename);
void db_close(TABLE* table);
void free_table(TABLE* table);
void* row_slot(TABLE* table, int row_num);
void print_student(STUDENT* student);
void table_insert(TABLE* table, STUDENT* student);
void table_select(TABLE* table);
STUDENT* make_student();
SEMESTER* make_semesters(STUDENT* student);
void to_upper_str(char* dest, const char* src);
void search_for_student(TABLE* table, char* student_to_find);
uint32_t get_next_student_id(TABLE* table);
void edit_student(TABLE* table, uint32_t student_id);
#endif