#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"
#define TEMP_NAME "temp.txt"

struct User {
    int id;
    char name[50];
    int age;
};

void createFile() {
    FILE *fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("Cannot create file.\n");
        exit(1);
    }
    fclose(fp);
}

int getInt(char *msg) {
    char line[100];
    int value;

    while (1) {
        printf("%s", msg);
        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(0);
        if (sscanf(line, "%d", &value) == 1)
            return value;
        printf("Please enter a number.\n");
    }
}

void getName(char *name) {
    int i, len;

    while (1) {
        printf("Enter name: ");
        if (fgets(name, 50, stdin) == NULL)
            exit(0);
        len = strlen(name);
        if (len > 0 && name[len - 1] == '\n')
            name[len - 1] = '\0';
        if (strlen(name) > 0)
            break;
        printf("Name cannot be empty.\n");
    }
    for (i = 0; name[i] != '\0'; i++) {
        if (name[i] == ',')
            name[i] = ' ';
    }
}

int readUser(FILE *fp, struct User *u) {
    char line[200];

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%d,%49[^,],%d", &u->id, u->name, &u->age) == 3)
            return 1;
    }
    return 0;
}

int findUser(int id) {
    FILE *fp = fopen(FILE_NAME, "r");
    struct User u;

    if (fp == NULL)
        return 0;
    while (readUser(fp, &u)) {
        if (u.id == id) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void addUser() {
    struct User u;
    FILE *fp;

    u.id = getInt("Enter ID: ");
    if (u.id <= 0) {
        printf("ID must be greater than 0.\n");
        return;
    }
    if (findUser(u.id)) {
        printf("This ID already exists.\n");
        return;
    }
    getName(u.name);
    u.age = getInt("Enter age: ");
    if (u.age <= 0) {
        printf("Age must be greater than 0.\n");
        return;
    }

    fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("Cannot open file.\n");
        return;
    }
    fprintf(fp, "%d,%s,%d\n", u.id, u.name, u.age);
    fclose(fp);
    printf("User added.\n");
}

void showUsers() {
    FILE *fp = fopen(FILE_NAME, "r");
    struct User u;
    int count = 0;

    if (fp == NULL) {
        printf("Cannot open file.\n");
        return;
    }
    printf("\n%-6s %-20s %s\n", "ID", "Name", "Age");
    printf("-------------------------------\n");
    while (readUser(fp, &u)) {
        printf("%-6d %-20s %d\n", u.id, u.name, u.age);
        count++;
    }
    if (count == 0)
        printf("No users found.\n");
    fclose(fp);
}

void updateUser() {
    FILE *fp, *temp;
    struct User u;
    char newName[50];
    int id, newAge;

    id = getInt("Enter ID to update: ");
    if (!findUser(id)) {
        printf("User not found.\n");
        return;
    }
    getName(newName);
    newAge = getInt("Enter new age: ");
    if (newAge <= 0) {
        printf("Age must be greater than 0.\n");
        return;
    }

    fp = fopen(FILE_NAME, "r");
    temp = fopen(TEMP_NAME, "w");
    if (fp == NULL || temp == NULL) {
        printf("Cannot open file.\n");
        return;
    }
    while (readUser(fp, &u)) {
        if (u.id == id)
            fprintf(temp, "%d,%s,%d\n", id, newName, newAge);
        else
            fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename(TEMP_NAME, FILE_NAME);
    printf("User updated.\n");
}

void deleteUser() {
    FILE *fp, *temp;
    struct User u;
    int id;

    id = getInt("Enter ID to delete: ");
    if (!findUser(id)) {
        printf("User not found.\n");
        return;
    }

    fp = fopen(FILE_NAME, "r");
    temp = fopen(TEMP_NAME, "w");
    if (fp == NULL || temp == NULL) {
        printf("Cannot open file.\n");
        return;
    }
    while (readUser(fp, &u)) {
        if (u.id != id)
            fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }
    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename(TEMP_NAME, FILE_NAME);
    printf("User deleted.\n");
}

int main() {
    int choice;

    createFile();

    while (1) {
        printf("\n1. Add user\n2. Show users\n3. Update user\n4. Delete user\n5. Exit\n");
        choice = getInt("Enter choice: ");

        if (choice == 1)
            addUser();
        else if (choice == 2)
            showUsers();
        else if (choice == 3)
            updateUser();
        else if (choice == 4)
            deleteUser();
        else if (choice == 5)
            break;
        else
            printf("Invalid choice.\n");
    }
    return 0;
}