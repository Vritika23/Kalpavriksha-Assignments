 Assignment-2
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

# Kalpavriksha Assignments

Assignment submissions for the **Kalpavriksha Program (Group E)**, written in **C**.

**Author:** Vritika Dadhich
**GitHub:** [vritika23](https://github.com/vritika23)
**HackerRank:** [vritikadadhich](https://www.hackerrank.com/profile/vritikadadhich)

## Assignment-1

| Program | Description | File |
|---------|-------------|------|
| Calculator | Evaluates an expression with `+ - * /` as an integer, following DMAS order, using two stacks. Reports invalid input and division by zero. | `Assignment-1/Calculator/calculator.c` |
| CRUD Operations | Create, read, update and delete user records (ID, name, age) stored in `users.txt`, using a `struct` and a temp file. | `Assignment-1/CRUD_Operations/crud.c` |

## Submission

- Every assignment is on its own branch, and a Pull Request is raised into `main`.
- `main` has only this README until the reviews are done.

## Compile and Run

```bash
gcc filename.c -o program
./program main
```
