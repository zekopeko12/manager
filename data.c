#include "data.h"

PAGER* open_pager(const char* filename) {
    int fd = open(  filename, 
                    O_RDWR |    // read/write
                    O_CREAT,    // kreiraj ako ne postoji
                    S_IWUSR |   // dopuštenje za pisanje
                    S_IRUSR     // dopuštenje za čitanje
                );

    
    if(fd == -1){
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);

    PAGER* pager = malloc(sizeof(PAGER));
    pager->file = fd;
    pager->file_length = file_length;
    pager->num_pages = file_length / PAGE_SIZE;

    if(file_length % PAGE_SIZE != 0) {
        pager->num_pages++;
    }

    memset(pager->pages, 0, sizeof(pager->pages));

    return pager;
}

void serialize_row(STUDENT* source, void* destination){
    if(source->semester_count > MAX_SEMESTERS || source->semester_count <= 0){
        printf("Invalid semester count (serialization): %d\n");
        exit(EXIT_FAILURE);
    }

    uint32_t semester_size = source->semester_count * sizeof(SEMESTER);

    memcpy(destination + STUDENT_AGE_OFFSET, &(source->age), STUDENT_AGE_SIZE);
    memcpy(destination + STUDENT_SEMESTER_COUNT_OFFSET, &(source->semester_count), STUDENT_SEMESTER_COUNT_SIZE);
    memcpy(destination + STUDENT_NAME_OFFSET, source->name, STUDENT_NAME_SIZE);
    memcpy(destination + STUDENT_ID_OFFSET, &(source->id), STUDENT_ID_SIZE);
    memcpy(destination + STUDENT_FIXED_SIZE, source->semesters, semester_size);
}

void deserialize_row(void* source, STUDENT* destination){
    memcpy(&(destination->age), source + STUDENT_AGE_OFFSET,  STUDENT_AGE_SIZE);
    memcpy(&(destination->semester_count), source + STUDENT_SEMESTER_COUNT_OFFSET,  STUDENT_SEMESTER_COUNT_SIZE);
    memcpy(destination->name, source + STUDENT_NAME_OFFSET, STUDENT_NAME_SIZE);
    memcpy(&(destination->id), source + STUDENT_ID_OFFSET, STUDENT_ID_SIZE);
    
    if(destination->semester_count > MAX_SEMESTERS || destination->semester_count <= 0){
        printf("Invalid semester count(deserialization): %d\n", destination->semester_count);
        exit(EXIT_FAILURE);
    }

    uint32_t semester_size = destination->semester_count * sizeof(SEMESTER);
    destination->semesters = malloc(semester_size);
    if(destination->semesters == NULL){
        printf("Failed to allocate memory for semesters.\n");
        exit(EXIT_SUCCESS);
    }

    memcpy(destination->semesters, source + STUDENT_FIXED_SIZE, semester_size);
}

int row_page(int row_num) {
    return row_num / ROWS_PER_PAGE;
}

int row_offset(int row_num) {
    return row_num % ROWS_PER_PAGE;
}

void* get_page(PAGER* pager, int page_num) {
    if(page_num >= MAX_PAGES){
        printf("Page number out of bounds!\n");
        exit(EXIT_SUCCESS);
    }

    if(pager->pages[page_num] == NULL){
        void* page = malloc(PAGE_SIZE);
        int num_pages = pager->file_length / PAGE_SIZE;

        if(pager->file_length % PAGE_SIZE){
            num_pages += 1;
        }

        if(page_num <= num_pages){
            lseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file, page, PAGE_SIZE);
            if(bytes_read == -1){
                printf("Error reading the file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void flush_page(PAGER* pager, int page_num, int size){
    if(pager->pages[page_num] == NULL) return;

    off_t offset = lseek(pager->file, page_num * PAGE_SIZE, SEEK_SET);
    if(offset == -1){
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->file, pager->pages[page_num], size);

    if(bytes_written == -1){
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

TABLE* db_open(const char* filename){
    TABLE* table = (TABLE*)malloc(sizeof(TABLE));
    if(table == NULL){
        printf("Failed to allocate table\n");
        exit(EXIT_FAILURE);
    }

    table->pager = open_pager(filename);
    int num_rows = table->pager->file_length / ROW_SIZE;
    table->num_rows = num_rows;

    return table;
}

void db_close(TABLE* table){
    PAGER* pager = table->pager;
    int num_of_full_pages = table->num_rows / ROWS_PER_PAGE;

    for(int i=0; i<num_of_full_pages; i++){
        if(pager->pages[i] == NULL){
            continue;
        }

        flush_page(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }


    //u slučaju da se treba spremiti parcijalna stranica (page) na kraj datoteke 
    int num_of_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if(num_of_additional_rows > 0){
        int page_num = num_of_full_pages;
        if(pager->pages[page_num] != NULL){
            flush_page(pager, page_num, num_of_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file);
    if(result == -1){
        printf("Error closing file\n");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<MAX_PAGES; i++){
        void* page = pager->pages[i];
        if(page){
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}

void free_table(TABLE* table){
    if(table == NULL) return;

    if(table->pager != NULL){   
        for(int i=0; i<MAX_PAGES; i++){
            if(table->pager->pages[i] != NULL){
                free(table->pager->pages[i]);
            }
        }

        free(table->pager);
    }

    free(table);
}

void* row_slot(TABLE* table, int row_num){
    int page_num = row_num / ROWS_PER_PAGE;
    void* page = get_page(table->pager, page_num);

    int row_offset = row_num % ROWS_PER_PAGE;
    int byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

void print_student(STUDENT* student){
    printf("\n---------------------------------------------\n");
    
    printf("\nID: %d | Name:%s | Age: %d", student->id, student->name, student->age);

    for(int i=0; i<student->semester_count; i++){
        printf("\n\tSemester: %d (Passed: %s)", i + 1, student->semesters[i].passed ? "Yes" : "No");
        for(int j=0; j<SUBJECTS_PER_SEMESTER; j++){
            printf("\n\tSubject name: %s | Grade: %d\n", student->semesters[i].subjects[j].name, student->semesters[i].subjects[j].grade);
        }
        printf("____________________________________________");
    }
}

void table_insert(TABLE* table, STUDENT* student){
    if(table->num_rows >= TABLE_MAX_ROWS){
        printf("Table is full.\n");
        return;
    }

    int row_num = table->num_rows;
    void* page = get_page(table->pager, row_page(row_num));
    void* destination = page + (row_offset(row_num) * ROW_SIZE);
    student->id = get_next_student_id(table);
    serialize_row(student, destination);

    table->num_rows++;
}

void table_select(TABLE* table){
    STUDENT student;
    bool empty = true;
    for(int i=0; i<table->num_rows; i++){
        deserialize_row(row_slot(table, i), &student);
        print_student(&student);
        empty = false;
        free(student.semesters);
    }

    if(empty){
        printf("\nDatabse is empty");
    }
}

STUDENT* make_student(){
    STUDENT* student = (STUDENT*)malloc(sizeof(STUDENT));
    if(student == NULL){
        printf("Failed to allocate student\n");
        exit(EXIT_SUCCESS);
    }

    getchar();
    printf("\nWhat is the name of the student?: ");
    fgets(student->name, 60, stdin);
    printf("\nHow old is the student?: ");
    scanf("%d", &student->age);
    printf("\nHow many semesters did the student attend?: ");
    do{
        scanf("%d", &student->semester_count);
    } while(student->semester_count > MAX_SEMESTERS || student->semester_count < 0);

    student->semesters = make_semesters(student);

    return student;
}

SEMESTER* make_semesters(STUDENT* student){
    SEMESTER* semester = (SEMESTER*)malloc(sizeof(SEMESTER) * student->semester_count);
    if(semester == NULL){
        printf("Failed to allocate semester\n");
        exit(EXIT_SUCCESS);
    }

    for(int i=0; i<student->semester_count; i++){
        semester[i].passed = true;
        printf("\nCurrently inputing %dth semester.", i+1);
        for(int j=0; j<SUBJECTS_PER_SEMESTER; j++){
            printf("\n\tName of subject: ");
            getchar();
            fgets(semester[i].subjects[j].name, 59, stdin);
            printf("\n\tGrade achieved: ");

            do{
                scanf("%d", &semester[i].subjects[j].grade);
            } while(semester[i].subjects[j].grade > 5 || semester[i].subjects[j].grade < 1);

            if(semester[i].subjects[j].grade == 1){
                semester[i].passed = false;
            }
        }
    }

    return semester;
}

void to_upper_str(char* dest, const char* src){
    for (int i = 0; src[i] != '\0'; i++) {
        dest[i] = toupper((unsigned char)src[i]);
    }
    dest[strlen(src)] = '\0';
}

void search_for_student(TABLE* table, char* student_to_find){
    STUDENT student;
    char upper_input [STUDENT_NAME_SIZE];
    bool found_student = false;
    to_upper_str(upper_input, student_to_find);
    for(int i=0; i<table->num_rows; i++){
        deserialize_row(row_slot(table, i), &student);
        char upper_name[STUDENT_NAME_SIZE];
        to_upper_str(upper_name, student.name);

        if(strcmp(upper_input, upper_name) == 0){
            found_student = true;
            print_student(&student);
        }
        free(student.semesters);
    }

    if(!found_student){
        printf("Such a student doesnt exist");
    }
}

uint32_t get_next_student_id(TABLE* table){
    uint32_t max_id = 0;
    STUDENT student;
    for(int i=0; i<table->num_rows; i++){
        deserialize_row(row_slot(table, i), &student);
        if(student.id > max_id){
            max_id = student.id;
        }

        free(student.semesters);
    }

    return max_id + 1;
}

void edit_student(TABLE* table, uint32_t student_id){
    if(student_id < 0 || student_id >= table->num_rows){
        printf("Invalid student ID.\n");
        return;
    }

    STUDENT editing_student;
    deserialize_row(row_slot(table, student_id), &editing_student);

    int choice;
    do{
        printf("\nCurrently editing student: %s with id: %d\n", editing_student.name, editing_student.id);
        printf("1. Edit student name\n");
        printf("2. Edit student age\n");
        printf("3. Edit subject\n");
        printf("0. Save and exit.\n");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                printf("\nInput new student name: ");
                getchar();
                fgets(editing_student.name, 60, stdin);
                break;

            case 2:
                printf("\nInput new student age: ");
                scanf("%d", &editing_student.age);
                break;
            
            case 3:
                print_student(&editing_student);
                uint32_t semester_index = 1;

                if(editing_student.semester_count > 1){
                    printf("\nWhat semester would you like to choose? ");
                    scanf("%d", &semester_index);
                }
                
                uint32_t subject_index;
                printf("\nSelect the subject youd like to edit (1-6): ");
                scanf("%d", &subject_index);

                SUBJECT* edited_subject = &editing_student.semesters[semester_index - 1].subjects[subject_index - 1];

                printf("\nCurrently editing: %s (Grade: %d)", edited_subject->name, edited_subject->grade);
                char new_subject_name[60];
                char new_subject_grade[sizeof(uint32_t)];

                printf("\nEnter new subject name (leave empty if you want it left unchanged)");
                
                //C je glup i ostavi \n od prethodnog scanf()
                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                fgets(new_subject_name, sizeof(new_subject_name), stdin);
                new_subject_name[strcspn(new_subject_name, "\n")] = '\0';

                if(strlen(new_subject_name) == 0){
                    printf("\nName unchanged.");
                }

                if(strlen(new_subject_name) > 0){
                    strncpy(edited_subject->name, new_subject_name, SUBJECT_NAME_SIZE - 1);
                    edited_subject->name[SUBJECT_NAME_SIZE - 1] = '\0';
                }

                int valid_input = 0;

                do{
                    printf("\nEnter new subject grade (leave empty if you want it left unchanged)");
                    fgets(new_subject_grade, sizeof(new_subject_grade), stdin);
                    new_subject_grade[strcspn(new_subject_grade, "\n")] = '\0';
    
                    if(strlen(new_subject_grade) == 0){
                        printf("\nGrade unchanged.");
                        break;
                    }
    
                    char* endptr;
                    long parsed_grade = strtol(new_subject_grade, &endptr, 10);

                    if(endptr == new_subject_grade || parsed_grade < 1 || parsed_grade > 5){
                        printf("\nInvalid grade. Input a grade in range [1-5]");
                    } else{
                        edited_subject->grade = (uint32_t)parsed_grade;
                        valid_input = 1;
                    }

                } while(!valid_input);

                flush_page(table->pager, student_id, PAGE_SIZE);

                break;

            case 0:
                break;

            default:
                printf("Invalid choice.\n");
        } 

    } while(choice != 0);

    serialize_row(&editing_student, row_slot(table, student_id));
    free(editing_student.semesters);
}

void delete_student(TABLE* table, uint32_t student_to_delete){
    if(student_to_delete < 0 || student_to_delete >= table->num_rows){
        printf("Invalid student ID.\n");
        return;
    }

    for(int i=student_to_delete; i<table->num_rows - 1; i++){
        void* src = row_slot(table, i + 1);
        void* dest = row_slot(table, i);
        memcpy(dest, src, ROW_SIZE);
    }

    table->num_rows--;
}