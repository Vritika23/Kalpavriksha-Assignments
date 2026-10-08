#include <stdio.h>

struct Student
{
    int rollNumber;
    char name[50];
    int marks[3];
};

int totalStudentsProcessed = 0;

int calculateTotal(struct Student student)
{
    int totalMarks = 0;
    for (int subjectIndex = 0; subjectIndex < 3; subjectIndex++)
    {
        totalMarks += student.marks[subjectIndex];
    }
    return totalMarks;
}

float calculateAverage(int totalMarks)
{
    return totalMarks / 3.0;
}

char assignGrade(float averageMarks)
{
    if (averageMarks >= 85)
    {
        return 'A';
    }
    else if (averageMarks >= 70)
    {
        return 'B';
    }
    else if (averageMarks >= 50)
    {
        return 'C';
    }
    else if (averageMarks >= 35)
    {
        return 'D';
    }
    return 'F';
}

int getStarCount(char grade)
{
    switch (grade)
    {
        case 'A':
            return 5;
        case 'B':
            return 4;
        case 'C':
            return 3;
        case 'D':
            return 2;
        default:
            return 0;
    }
}

void printStars(int starCount)
{
    for (int starIndex = 0; starIndex < starCount; starIndex++)
    {
        printf("*");
    }
    printf("\n");
}

void printRollNumbers(struct Student students[], int currentIndex, int studentCount)
{
    if (currentIndex == studentCount)
    {
        return;
    }
    printf("%d ", students[currentIndex].rollNumber);
    printRollNumbers(students, currentIndex + 1, studentCount);
}

int readMarks(int subjectNumber)
{
    int marks;
    do
    {
        printf("Enter marks in subject %d (0-100): ", subjectNumber);
        scanf("%d", &marks);
    } while (marks < 0 || marks > 100);
    return marks;
}

int main()
{
    int studentCount;
    struct Student students[100];
    do
    {
        printf("Enter number of students (1-100): ");
        scanf("%d", &studentCount);
    } while (studentCount < 1 || studentCount > 100);
    for (int studentIndex = 0; studentIndex < studentCount; studentIndex++)
    {
        printf("\nEnter details for student %d\n", studentIndex + 1);
        printf("Enter roll number: ");
        scanf("%d", &students[studentIndex].rollNumber);
        printf("Enter name: ");
        scanf("%s", students[studentIndex].name);
        for (int subjectIndex = 0; subjectIndex < 3; subjectIndex++)
        {
            students[studentIndex].marks[subjectIndex] = readMarks(subjectIndex + 1);
        }
    }
    printf("\n----- Student Performance Report -----\n\n");
    for (int studentIndex = 0; studentIndex < studentCount; studentIndex++)
    {
        int totalMarks = calculateTotal(students[studentIndex]);
        float averageMarks = calculateAverage(totalMarks);
        char grade = assignGrade(averageMarks);
        totalStudentsProcessed++;
        printf("Roll: %d\n", students[studentIndex].rollNumber);
        printf("Name: %s\n", students[studentIndex].name);
        printf("Total: %d\n", totalMarks);
        printf("Average: %.2f\n", averageMarks);
        printf("Grade: %c\n", grade);
        if (averageMarks < 35)
        {
            printf("\n");
            continue;
        }
        printf("Performance: ");
        printStars(getStarCount(grade));
        printf("\n");
    }
    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, 0, studentCount);
    printf("\n");
    return 0;
}
