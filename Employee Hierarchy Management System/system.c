#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define NAME_SIZE 100
#define DEG_SIZE 50
#define LINE_SIZE 100

#define INPUT_OK 1
#define INPUT_TOO_LONG 0
#define INPUT_EOF -1
#define RESULT_NOMEM -2

struct Employee {
    int id;
    char name[NAME_SIZE];
    char designation[DEG_SIZE];
    struct Employee *manager;
    struct Employee *firstReport;
    struct Employee *nextReport;
};

int readLine(char *buf, size_t size) {
    if(fgets(buf, (int)size, stdin) == NULL)
        return INPUT_EOF;

    size_t len = strlen(buf);

    if(len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        return INPUT_OK;
    }

    int ch = getchar();

    if(ch == '\n' || ch == EOF)
        return INPUT_OK;

    while(ch != '\n' && ch != EOF)
        ch = getchar();

    return INPUT_TOO_LONG;
}

int inputInt(const char *prompt, int *value) {
    char input[LINE_SIZE];

    while(1) {
        printf("%s", prompt);

        int status = readLine(input, sizeof(input));

        if(status == INPUT_EOF)
            return INPUT_EOF;

        if(status == INPUT_TOO_LONG) {
            printf("\nInput too long!\n");
            continue;
        }

        int n;
        char extra;

        if(sscanf(input, "%d %c", &n, &extra) != 1) {
            printf("\nEnter only integer!\n");
            continue;
        }

        *value = n;
        return INPUT_OK;
    }
}

int inputText(const char *prompt, char *dest, size_t size) {
    char input[NAME_SIZE > DEG_SIZE ? NAME_SIZE : DEG_SIZE];

    while(1) {
        printf("%s", prompt);

        int status = readLine(input, size);

        if(status == INPUT_EOF)
            return INPUT_EOF;

        if(status == INPUT_TOO_LONG) {
            printf("\nInput too long! Maximum %zu characters.\n", size - 1);
            continue;
        }

        int found = 0;
        int valid = 1;

        for(int i = 0; input[i] != '\0'; i++) {
            unsigned char c = (unsigned char)input[i];

            if(!isalpha(c) && !isspace(c)) {
                valid = 0;
                break;
            }

            if(!isspace(c))
                found = 1;
        }

        if(!valid) {
            printf("\nEnter only Letters\n");
            continue;
        }

        if(!found) {
            printf("\nString can't be empty!\n");
            continue;
        }

        strcpy(dest, input);
        return INPUT_OK;
    }
}

int inputName(struct Employee *e) {
    return inputText("Enter employee name: ", e->name, NAME_SIZE);
}

int inputDesignation(struct Employee *e) {
    return inputText("Enter designation: ", e->designation, DEG_SIZE);
}

struct Employee *searchEmployee(struct Employee *e, int id) {
    if(e == NULL)
        return NULL;

    if(e->id == id)
        return e;

    struct Employee *a = searchEmployee(e->firstReport, id);

    if(a != NULL)
        return a;

    return searchEmployee(e->nextReport, id);
}

int countReports(struct Employee *e) {
    if(e == NULL)
        return 0;

    return 1 + countReports(e->nextReport);
}

int countEmployees(struct Employee *root) {
    if(root == NULL)
        return 0;

    return countEmployees(root->firstReport)
         + countEmployees(root->nextReport)
         + 1;
}

int countEmployeesUnder(struct Employee *e) {
    if(e == NULL)
        return 0;

    return countEmployees(e->firstReport);
}

struct Employee *findLastEmployee(struct Employee *e) {
    if(e == NULL || e->nextReport == NULL)
        return e;

    return findLastEmployee(e->nextReport);
}

struct Employee *findBestManager(
    struct Employee *curr,
    struct Employee *target,
    struct Employee *best,
    int *minReports
) {
    if(curr == NULL)
        return best;

    if(curr != target) {
        int count = countReports(curr->firstReport);

        if(count < *minReports) {
            *minReports = count;
            best = curr;
        }
    }

    return findBestManager(curr->nextReport, target, best, minReports);
}

void changeManager(struct Employee *e, struct Employee *manager) {
    if(e == NULL)
        return;

    e->manager = manager;
    changeManager(e->nextReport, manager);
}

void unlinkReport(struct Employee **head, struct Employee *target) {
    if(*head == NULL)
        return;

    if(*head == target) {
        *head = target->nextReport;
        return;
    }

    unlinkReport(&((*head)->nextReport), target);
}

void linkReport(struct Employee *manager, struct Employee *e) {
    e->manager = manager;
    e->nextReport = manager->firstReport;
    manager->firstReport = e;
}

struct Employee *createEmployee(int id) {
    struct Employee *e = malloc(sizeof(struct Employee));

    if(e == NULL)
        return NULL;

    e->id = id;
    e->name[0] = '\0';
    e->designation[0] = '\0';
    e->manager = NULL;
    e->firstReport = NULL;
    e->nextReport = NULL;
    return e;
}

void freeHierarchy(struct Employee *e) {
    if(e == NULL)
        return;

    freeHierarchy(e->firstReport);
    freeHierarchy(e->nextReport);
    free(e);
}

int readEmployeeDetails(struct Employee *e) {
    if(inputName(e) == INPUT_EOF)
        return INPUT_EOF;

    if(inputDesignation(e) == INPUT_EOF)
        return INPUT_EOF;

    return INPUT_OK;
}

int readNewId(struct Employee *root, int *id) {
    int value;

    if(inputInt("\nEnter employee ID: ", &value) == INPUT_EOF)
        return INPUT_EOF;

    if(value <= 0) {
        printf("\nID must be greater than 0\n");
        return 0;
    }

    if(searchEmployee(root, value) != NULL) {
        printf("Employee ID already exists!\n");
        return 0;
    }

    *id = value;
    return 1;
}

int insertEmployee(struct Employee **root) {
    int id;
    int status = readNewId(*root, &id);

    if(status != 1)
        return status;

    struct Employee *newEmployee = createEmployee(id);

    if(newEmployee == NULL) {
        printf("Memory allocation failed!\n");
        return RESULT_NOMEM;
    }

    if(readEmployeeDetails(newEmployee) == INPUT_EOF) {
        free(newEmployee);
        return INPUT_EOF;
    }

    if(*root == NULL) {
        *root = newEmployee;
        printf("CEO added successfully!\n");
        return 1;
    }

    int managerId;

    if(inputInt("Enter reporting manager ID: ", &managerId) == INPUT_EOF) {
        free(newEmployee);
        return INPUT_EOF;
    }

    struct Employee *manager = searchEmployee(*root, managerId);

    if(manager == NULL) {
        printf("Manager not found!\n");
        free(newEmployee);
        return 0;
    }

    linkReport(manager, newEmployee);

    printf("Employee added successfully!\n");
    return 1;
}

void reassignReportees(
    struct Employee *reportees,
    struct Employee *best,
    struct Employee *manager
) {
    if(reportees == NULL)
        return;

    struct Employee *destination = (best != NULL) ? best : manager;

    changeManager(reportees, destination);

    struct Employee *last = findLastEmployee(reportees);
    last->nextReport = destination->firstReport;
    destination->firstReport = reportees;
}

int deleteEmployee(struct Employee **root, int id) {
    struct Employee *target = searchEmployee(*root, id);

    if(target == NULL) {
        printf("\nEmployee doesn't exist\n");
        return 0;
    }

    if(*root == target) {
        printf("\nCEO can't be removed\n");
        return 0;
    }

    struct Employee *manager = target->manager;
    struct Employee *reportees = target->firstReport;
    int minReports = INT_MAX;

    struct Employee *best = findBestManager(
        manager->firstReport, target, NULL, &minReports
    );

    target->firstReport = NULL;
    unlinkReport(&manager->firstReport, target);
    reassignReportees(reportees, best, manager);

    free(target);

    printf("\nEmployee removed successfully.\n");
    return 1;
}

void showHierarchy(struct Employee *root, int showSibling) {
    if(root == NULL)
        return;

    printf("%d - %s - %s\n", root->id, root->name, root->designation);

    showHierarchy(root->firstReport, 1);

    if(showSibling)
        showHierarchy(root->nextReport, 1);
}

void showCompleteHierarchy(struct Employee *root) {
    if(root == NULL) {
        printf("\nNo employees yet.\n");
        return;
    }

    showHierarchy(root, 0);
    printf("Total Employees: %d\n", countEmployees(root));
}

void printMenu() {
    printf("\n1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n");
}

int handleAdd(struct Employee **root) {
    return insertEmployee(root);
}

int handleRemove(struct Employee **root) {
    int id;

    if(inputInt("\nEnter Employee ID to remove: ", &id) == INPUT_EOF)
        return INPUT_EOF;

    deleteEmployee(root, id);
    return 1;
}

int handleShowOne(struct Employee *root) {
    int id;

    if(inputInt("\nEnter Employee ID: ", &id) == INPUT_EOF)
        return INPUT_EOF;

    struct Employee *e = searchEmployee(root, id);

    if(e == NULL) {
        printf("Employee Not found!\n");
        return 1;
    }

    showHierarchy(e, 0);
    printf("Total employees reporting under %d: %d\n",
           e->id, countEmployeesUnder(e));
    return 1;
}

int main() {
    struct Employee *root = NULL;
    int exitCode = 0;
    int running = 1;

    while(running) {
        int choice;
        int result = 1;

        printMenu();

        if(inputInt("Enter choice: ", &choice) == INPUT_EOF) {
            printf("\nNo more input. Exiting.\n");
            break;
        }

        switch(choice) {
            case 1:
                result = handleAdd(&root);
                break;

            case 2:
                result = handleRemove(&root);
                break;

            case 3:
                result = handleShowOne(root);
                break;

            case 4:
                showCompleteHierarchy(root);
                break;

            case 5:
                running = 0;
                break;

            default:
                printf("Enter options from 1 - 5\n");
        }

        if(result == INPUT_EOF) {
            printf("\nNo more input. Exiting.\n");
            break;
        }

        if(result == RESULT_NOMEM) {
            exitCode = 1;
            break;
        }
    }

    freeHierarchy(root);
    return exitCode;
}
