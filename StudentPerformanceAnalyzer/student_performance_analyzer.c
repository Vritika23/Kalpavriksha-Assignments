#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

struct Student {
    char name[50];
    int rollno;
    int marks[3];
};

// Calculate total marks
int calculateTotal(struct Student *arr) {
    int sum = 0;
    for(int i = 0; i < 3; i++) {
        sum += arr->marks[i];
    }
    return sum;
}

// Calculate average marks
float calculateAverage(int totalMarks) {
    return totalMarks / 3.0;
}

// Assign grade based on average
char calculateGrade(float avgMarks) {
    if(avgMarks >= 85) return 'A';
    else if(avgMarks >= 70) return 'B';
    else if(avgMarks >= 50) return 'C';
    else if(avgMarks >= 35) return 'D';
    else return 'F';
}

// Get stars for grade
int getStarCount(char grade) {
    int n = 0;
    switch(grade) {
        case 'A': n = 5;
        break;
        case 'B': n = 4;
        break;
        case 'C': n = 3;
        break;
        case 'D': n = 2;
        break;
        default: n = 0;
    }
    return n;
}

// Print star pattern
void printStars(int n) {
    for(int i = 0; i < n; i++) {
        printf("*");
    }
}

// Print roll numbers recursively
void displayRollNumbers(struct Student *arr, int i, int n) {
    if(i >= n) return;
    printf("%d ", (arr + i)->rollno);
    displayRollNumbers(arr, i + 1, n);
}

// Read student count
int readStudentCount() {
    int n;
    char input[100];
    char extra;

    while(1) {
        printf("Enter Number of Students: ");
        if(fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error Reading the input!");
            return -1;
        }

        if(sscanf(input, "%d %c", &n, &extra) != 1) {
            printf("Invalid input! Enter an Integer Only!\n");
            continue;
        }

        // Validate student count
        if(n < 1 || n > 100) {
            printf("Number of Students must lie between 1 - 100\n");
            continue;
        }

        break;
    }
    return n;
}

// Validate marks
int validateMarks(int marks) {
    if(marks < 0 || marks > 100) {
        printf("Marks must be between 0 and 100.\n");
        return 0;
    }
    return 1;
}

// Validate student name
int validateName(char *name) {
    for(int i = 0; name[i] != '\0'; i++) {
        if(!isalpha((unsigned char) name[i])) return 0;
    }
    return 1;
}

// Read student details
int readStudentDetails(struct Student *arr, int n) {
    char input[200];
    char extra;

    for(int i = 0; i < n; i++) {
        while(1) {
            printf("Enter RollNo%d, Name%d, Marks1, Marks2, Marks3: ", i + 1, i + 1);

            if(fgets(input, sizeof(input), stdin) == NULL) {
                printf("Error Taking input!");
                return 0;
            }

            if(sscanf(input, "%d %49s %d %d %d %c",
                &arr[i].rollno, arr[i].name,
                &arr[i].marks[0], &arr[i].marks[1],
                &arr[i].marks[2], &extra) != 5) {
                printf("Enter all 5 fields correctly!\n");
                continue;
            }

            if(!validateName(arr[i].name)) {
                printf("Invalid Name! Use alphbetic Characters only.\n");
                continue;
            }

            if(arr[i].rollno < 1) {
                printf("Roll Number must be atleast 1.\n");
                continue;
            }

            int valid = 1;

            for(int j = 0; j < 3; j++) {
                if(!validateMarks(arr[i].marks[j])) {
                    valid = 0;
                    break;
                }
            }

            if(!valid) {
                printf("Please re-enter all five fields.\n");
                continue;
            }

            break;
        }
    }
    return 1;
}

// Display student reports
void printStudentReports(struct Student *arr, int n) {
    for(int i = 0; i < n; i++) {
        // Print student details
        printf("\nRoll No.: %d\nName: %s", arr[i].rollno, arr[i].name);

        // Calculate total
        int totalmarks = calculateTotal(&arr[i]);
        printf("\nTotal Marks: %d", totalmarks);

        // Calculate average
        float avgmarks = calculateAverage(totalmarks);
        printf("\nAverage Marks: %f", avgmarks);

        // Calculate grade
        char grade = calculateGrade(avgmarks);
        printf("\nGrade: %c", grade);

        // Print performance pattern
        if(avgmarks < 35) {
            printf("\nStudent's average below 35, so no pattern print!\n");
            continue;
        }

        int numStars = getStarCount(grade);
        printf("\nPerformance Pattern: ");
        printStars(numStars);
        printf("\n");
    }
}

int main() {
    // Read student count
    int n = readStudentCount();
    if(n == -1) return 0;

    struct Student *arr = malloc(n * sizeof(*arr));

    if(arr == NULL) {
        printf("Memory Allocation failed!");
        return 1;
    }

    // Read student details
    if(!readStudentDetails(arr, n)) {
        printf("Error Reading Students Data");
        free(arr);
        return 1;
    }

    // Print student reports
    printStudentReports(arr, n);

    // Print roll numbers recursively
    printf("\nList of Roll Numbers: ");
    displayRollNumbers(arr, 0, n);

    // Release allocated memory
    free(arr);
    return 0;
}
