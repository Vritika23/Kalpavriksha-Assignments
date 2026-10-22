#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define NAME_SIZE 100
#define DEG_SIZE 50

struct Employee {
    int id;
    char name[NAME_SIZE];
    char designation[DEG_SIZE];
    struct Employee *manager;
    struct Employee *firstReport;
    struct Employee *nextReport;
};

int inputInt() {
    char input[NAME_SIZE];
    char extra;
    int n;

    if(fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error taking input");
        return -1;
    }

    if(sscanf(input, "%d %c", &n, &extra) != 1) {
        printf("\nEnter only integer!");
        printf("\nEnter Employee ID: ");
        return inputInt();
    }

    return n;
}

int inputName(struct Employee *e) {
    char input[NAME_SIZE];

    if(fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error taking Input");
        return -1;
    }

    input[strcspn(input, "\n")] = '\0';
    int found = 0;

    for(int i = 0; input[i] != '\0'; i++) {
        char c = (unsigned char)input[i];

        if(!isalpha(c) && !isspace(c)) {
            printf("\nEnter only Letters");
            printf("\nEnter: ");
            return inputName(e);
        }

        if(!isspace(c))
            found = 1;
    }

    if(!found) {
        printf("\nString can't be empty!");
        return inputName(e);
    }

    strcpy(e->name, input);
    return 1;
}

int inputDesignation(struct Employee *e) {
    char input[DEG_SIZE];

    if(fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error taking Input");
        return -1;
    }

    input[strcspn(input, "\n")] = '\0';
    int found = 0;

    for(int i = 0; input[i] != '\0'; i++) {
        char c = (unsigned char)input[i];

        if(!isalpha(c) && !isspace(c)) {
            printf("\nEnter only Letters");
            printf("\nEnter: ");
            return inputDesignation(e);
        }

        if(!isspace(c))
            found = 1;
    }

    if(!found) {
        printf("\nString can't be empty!");
        return inputDesignation(e);
    }

    strcpy(e->designation, input);
    return 1;
}

struct Employee *searchEmployee(struct Employee *e, int id) {
    if(e == NULL)
        return NULL;

    if(e->id == id)
        return e;

    struct Employee *a = searchEmployee(e->firstReport, id);
    struct Employee *b = searchEmployee(e->nextReport, id);

    return a != NULL ? a : b;
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
    e->manager = NULL;
    e->firstReport = NULL;
    e->nextReport = NULL;
    return e;
}

int readEmployeeDetails(struct Employee *e) {
    printf("Enter employee name: ");
    if(inputName(e) == -1)
        return -1;

    printf("Enter designation: ");
    if(inputDesignation(e) == -1)
        return -1;

    return 1;
}

int readNewId(struct Employee *root) {
    printf("\nEnter employee ID: ");
    int id = inputInt();

    if(id == -1)
        return -1;

    if(id <= 0) {
        printf("\nID must be greater than 0");
        return 0;
    }

    if(searchEmployee(root, id) != NULL) {
        printf("Employee ID already exists!\n");
        return 0;
    }

    return id;
}

int insertEmployee(struct Employee **root) {
    int id = readNewId(*root);

    if(id <= 0)
        return id;

    struct Employee *newEmployee = createEmployee(id);

    if(newEmployee == NULL) {
        printf("Memory allocation failed!\n");
        return -1;
    }

    if(readEmployeeDetails(newEmployee) == -1) {
        free(newEmployee);
        return -1;
    }

    if(*root == NULL) {
        *root = newEmployee;
        printf("CEO added successfully!\n");
        return 1;
    }

    printf("Enter reporting manager ID: ");
    int managerId = inputInt();

    if(managerId == -1) {
        free(newEmployee);
        return -1;
    }

    struct Employee *manager = searchEmployee(*root, managerId);

    if(manager == NULL) {
        printf("Manager not found!\n");
        free(newEmployee);
        return 0;
    }

    if(manager == newEmployee) {
        printf("An employee cannot manage themselves!\n");
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
        printf("\nEmployee Don't Exits");
        return 0;
    }

    if(*root == target) {
        printf("\nCEO Can't be removed");
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

    printf("\nEmployee removed successfully.");
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
    if(root == NULL)
        return;

    showHierarchy(root, 0);
    printf("Total Employees: %d\n", countEmployees(root));
}

void printMenu() {
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n");
}

int handleAdd(struct Employee **root) {
    return insertEmployee(root);
}

void handleRemove(struct Employee **root) {
    printf("\nEnter Employee ID to remove: ");
    int id = inputInt();

    deleteEmployee(root, id);
}

void handleShowOne(struct Employee *root) {
    printf("\nEnter Employee ID: ");
    int id = inputInt();

    if(id == -1)
        return;

    struct Employee *e = searchEmployee(root, id);

    if(e == NULL)
        printf("Employee Not found!");
    else
        showHierarchy(e, 0);
}

int main() {
    struct Employee *root = NULL;

    while(1) {
        printMenu();

        switch(inputInt()) {
            case 1:
                if(handleAdd(&root) == -1)
                    return 1;
                break;

            case 2:
                handleRemove(&root);
                break;

            case 3:
                handleShowOne(root);
                break;

            case 4:
                showCompleteHierarchy(root);
                break;

            case 5:
                return 0;

            default:
                printf("Enter options from 1 - 5\n");
        }
    }

    return 0;
}
