# Student Performance Analyzer

A console-based C program that reads student details and marks in three subjects, then shows the total, average, grade and a star pattern for each student.

## Grading

| Average | Grade | Stars |
|---------|-------|-------|
| >= 85   | A     | 5     |
| >= 70   | B     | 4     |
| >= 50   | C     | 3     |
| >= 35   | D     | 2     |
| < 35    | F     | none  |

## Concepts Used

Structures, arithmetic operators, if-else and switch-case, loops, `continue`, functions, recursion and variable scope.

## Run

```
gcc student_performance_analyzer.c -o student_performance_analyzer
./student_performance_analyzer
```

## Input

The program prompts for the number of students (1 to 100), then for each student: roll number, name (single word) and marks in three subjects (0 to 100). Invalid values are asked again.

## Sample Output

```
Roll: 1
Name: Arti
Total: 250
Average: 83.33
Grade: B
Performance: ****

List of Roll Numbers (via recursion): 1
```
