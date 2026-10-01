#include <stdio.h>
#include <ctype.h>

int numStack[1000];
char opStack[1000];
int numTop = -1;
int opTop = -1;
int divByZero = 0;

int precedence(char op) {
    if (op == '*' || op == '/')
        return 2;
    return 1;
}

void applyOperator() {
    int second = numStack[numTop--];
    int first = numStack[numTop--];
    char op = opStack[opTop--];
    int result = 0;

    if (op == '+')
        result = first + second;
    else if (op == '-')
        result = first - second;
    else if (op == '*')
        result = first * second;
    else {
        if (second == 0)
            divByZero = 1;
        else
            result = first / second;
    }
    numStack[++numTop] = result;
}

int main() {
    char expr[1000];
    int i, number;
    int expectNumber = 1;

    fgets(expr, 1000, stdin);

    for (i = 0; expr[i] != '\0' && expr[i] != '\n'; i++) {
        if (expr[i] == ' ' || expr[i] == '\t' || expr[i] == '"')
            continue;

        if (isdigit(expr[i])) {
            if (expectNumber == 0) {
                printf("Error: Invalid expression.\n");
                return 0;
            }
            number = 0;
            while (isdigit(expr[i])) {
                number = number * 10 + (expr[i] - '0');
                i++;
            }
            i--; 
            numStack[++numTop] = number;
            expectNumber = 0;
        }
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            if (expectNumber == 1) {
                printf("Error: Invalid expression.\n");
                return 0;
            }
            // if old operator is same or higher, solve it first
            while (opTop >= 0 && precedence(opStack[opTop]) >= precedence(expr[i]))
                applyOperator();
            opStack[++opTop] = expr[i];
            expectNumber = 1;
        }
        else {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    if (expectNumber == 1) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    while (opTop >= 0)
        applyOperator();

    // checking div by zero at end, invalid check is first
    if (divByZero == 1)
        printf("Error: Division by zero.\n");
    else
        printf("%d\n", numStack[numTop]);

    return 0;
}