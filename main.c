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
        printf("\n2. Input Students");
        printf("\n3. Exit\n");

        scanf("%d", &choice);
        
        if(choice == 3){
            printf("Exiting program...");
            db_close(table);
            exit(EXIT_SUCCESS);
        } else if(choice == 1){
            table_select(table);
        } else if(choice == 2){
            STUDENT* student = make_student();
            table_insert(table, student);           
        }        
    }
}