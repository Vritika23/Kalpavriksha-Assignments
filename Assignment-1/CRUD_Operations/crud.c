#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define FILE_NAME "users.txt"
#define TEMP_NAME "temp.txt"
#define NAME_SIZE 50
#define LINE_SIZE 100

#define INPUT_OK 1
#define INPUT_EOF 0
#define INPUT_TOO_LONG -1

struct User {
    int id;
    char name[NAME_SIZE];
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

// reads one line, removes the newline, flushes the rest if the line is too long
int readLine(char *buf, int size) {
    int c;
    size_t len;

    if (fgets(buf, size, stdin) == NULL)
        return INPUT_EOF;

    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        return INPUT_OK;
    }
    if (len == (size_t)(size - 1)) {
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return INPUT_TOO_LONG;
    }
    return INPUT_OK;
}

// returns 1 on success, 0 if input ended
int getInt(const char *msg, int *value) {
    char line[LINE_SIZE];
    char *end;
    long number;
    int status;

    while (1) {
        printf("%s", msg);
        status = readLine(line, sizeof(line));
        if (status == INPUT_EOF)
            return 0;
        if (status == INPUT_TOO_LONG) {
            printf("Input is too long.\n");
            continue;
        }

        errno = 0;
        number = strtol(line, &end, 10);
        if (end == line) {
            printf("Please enter a valid whole number.\n");
            continue;
        }
        while (isspace((unsigned char)*end))
            end++;
        if (*end != '\0') {
            printf("Please enter a valid whole number.\n");
            continue;
        }
        if (errno == ERANGE || number > INT_MAX || number < INT_MIN) {
            printf("Number is out of range.\n");
            continue;
        }

        *value = (int)number;
        return 1;
    }
}

// returns 1 on success, 0 if input ended
int getName(char *name) {
    char buf[NAME_SIZE + 1];
    int status, i;

    while (1) {
        printf("Enter name: ");
        status = readLine(buf, sizeof(buf));
        if (status == INPUT_EOF)
            return 0;
        if (status == INPUT_TOO_LONG) {
            printf("Name is too long (max %d characters).\n", NAME_SIZE - 1);
            continue;
        }
        if (strlen(buf) == 0) {
            printf("Name cannot be empty.\n");
            continue;
        }
        break;
    }

    for (i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == ',')
            buf[i] = ' ';
    }
    snprintf(name, NAME_SIZE, "%s", buf);
    return 1;
}

int readUser(FILE *fp, struct User *u) {
    char line[200];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 49 is NAME_SIZE - 1
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

// updated != NULL replaces the record, updated == NULL deletes it; returns 1 on success
int rewriteFile(int id, const struct User *updated) {
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen(TEMP_NAME, "w");
    struct User u;

    if (fp == NULL || temp == NULL) {
        if (fp != NULL)
            fclose(fp);
        if (temp != NULL) {
            fclose(temp);
            remove(TEMP_NAME);
        }
        printf("Cannot open file.\n");
        return 0;
    }

    while (readUser(fp, &u)) {
        if (u.id != id)
            fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
        else if (updated != NULL)
            fprintf(temp, "%d,%s,%d\n", updated->id, updated->name, updated->age);
        // delete case writes nothing
    }
    fclose(fp);

    if (fclose(temp) != 0) {
        printf("Error: could not finish writing the temp file.\n");
        remove(TEMP_NAME);
        return 0;
    }
    if (remove(FILE_NAME) != 0) {
        printf("Error: could not remove the old file.\n");
        remove(TEMP_NAME);
        return 0;
    }
    if (rename(TEMP_NAME, FILE_NAME) != 0) {
        // keep temp.txt here, it holds the only copy of the data
        printf("Error: could not rename %s to %s. Your data is in %s.\n",
               TEMP_NAME, FILE_NAME, TEMP_NAME);
        return 0;
    }
    return 1;
}

// returns 1 to carry on, 0 if input ended
int addUser() {
    struct User u;
    FILE *fp;

    if (!getInt("Enter ID: ", &u.id))
        return 0;
    if (u.id <= 0) {
        printf("ID must be greater than 0.\n");
        return 1;
    }
    if (findUser(u.id)) {
        printf("This ID already exists.\n");
        return 1;
    }
    if (!getName(u.name))
        return 0;
    if (!getInt("Enter age: ", &u.age))
        return 0;
    if (u.age <= 0) {
        printf("Age must be greater than 0.\n");
        return 1;
    }

    fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("Cannot open file.\n");
        return 1;
    }
    fprintf(fp, "%d,%s,%d\n", u.id, u.name, u.age);
    fclose(fp);
    printf("User added.\n");
    return 1;
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

// returns 1 to carry on, 0 if input ended
int updateUser() {
    struct User updated;

    if (!getInt("Enter ID to update: ", &updated.id))
        return 0;
    if (!findUser(updated.id)) {
        printf("User not found.\n");
        return 1;
    }
    if (!getName(updated.name))
        return 0;
    if (!getInt("Enter new age: ", &updated.age))
        return 0;
    if (updated.age <= 0) {
        printf("Age must be greater than 0.\n");
        return 1;
    }

    if (rewriteFile(updated.id, &updated))
        printf("User updated.\n");
    return 1;
}

// returns 1 to carry on, 0 if input ended
int deleteUser() {
    int id;

    if (!getInt("Enter ID to delete: ", &id))
        return 0;
    if (!findUser(id)) {
        printf("User not found.\n");
        return 1;
    }

    if (rewriteFile(id, NULL))
        printf("User deleted.\n");
    return 1;
}

int main() {
    int choice;
    int ok;

    createFile();

    while (1) {
        printf("\n1. Add user\n2. Show users\n3. Update user\n4. Delete user\n5. Exit\n");
        if (!getInt("Enter choice: ", &choice)) {
            fprintf(stderr, "Error: input ended unexpectedly.\n");
            return EXIT_FAILURE;
        }

        ok = 1;
        if (choice == 1)
            ok = addUser();
        else if (choice == 2)
            showUsers();
        else if (choice == 3)
            ok = updateUser();
        else if (choice == 4)
            ok = deleteUser();
        else if (choice == 5)
            break;
        else
            printf("Invalid choice.\n");

        if (!ok) {
            fprintf(stderr, "Error: input ended unexpectedly.\n");
            return EXIT_FAILURE;
        }
    }
    return 0;
}
