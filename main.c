#include "data.h"
#include <string.h>

int main(int argc, char *argv[]) {
    int choice;
    printf("Starting...");
    TABLE* table = db_open("data.db");
    printf("Opened table!");
    printf("\nHello!");

    while(true){
        printf("\nChoose what you would like to do:");
        printf("\n1. List the students");
        printf("\n2. Input students");
        printf("\n3. Search for student");
        printf("\n4. Edit student");
        printf("\n5. Delete student");
        printf("\n6. Exit\n");

        scanf("%d", &choice);

        switch(choice){
            case 1:
                table_select(table);
                break;

            case 2:
                STUDENT* student = make_student();
                table_insert(table, student);
                break;

            case 3:
                char student_to_search_for[60];
                printf("What student would you like to look for?\n");
                getchar();
                fgets(student_to_search_for, 60, stdin);
                search_for_student(table, student_to_search_for);
                break;

            case 4:
                uint32_t editing_student_id;
                printf("What student would you like to edit(ID)? ");
                scanf("%d", &editing_student_id);
                editing_student_id -= 1;
                edit_student(table, editing_student_id);
                break;

            case 5:
                uint32_t student_to_delete;
                printf("What student would you like to delete(ID)? ");
                scanf("%d", &student_to_delete);
                delete_student(table, student_to_delete - 1);
                break;
            
            case 6:
                printf("Exiting program...");
                db_close(table);
                exit(EXIT_SUCCESS);
                break;

            default:
                printf("Invalid choice.\n");
        }
    }
}