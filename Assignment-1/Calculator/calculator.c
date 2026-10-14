#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#define MAX_STACK_SIZE 1000
#define MAX_INPUT_SIZE 1000

typedef enum {
    CALC_OK = 0,
    CALC_ERR_INVALID,
    CALC_ERR_DIV_ZERO
} CalcStatus;

typedef struct {
    int numStack[MAX_STACK_SIZE];
    char opStack[MAX_STACK_SIZE];
    int numTop;
    int opTop;
} Calculator;

void initCalculator(Calculator *calc) {
    calc->numTop = -1;
    calc->opTop = -1;
}

int precedence(char op) {
    if (op == '*' || op == '/')
        return 2;
    return 1;
}

CalcStatus pushNumber(Calculator *calc, int value) {
    if (calc->numTop >= MAX_STACK_SIZE - 1)
        return CALC_ERR_INVALID;
    calc->numStack[++calc->numTop] = value;
    return CALC_OK;
}

CalcStatus pushOperator(Calculator *calc, char op) {
    if (calc->opTop >= MAX_STACK_SIZE - 1)
        return CALC_ERR_INVALID;
    calc->opStack[++calc->opTop] = op;
    return CALC_OK;
}

CalcStatus applyOperator(Calculator *calc) {
    // need at least 2 numbers and 1 operator
    if (calc->numTop < 1 || calc->opTop < 0)
        return CALC_ERR_INVALID;

    int second = calc->numStack[calc->numTop--];
    int first = calc->numStack[calc->numTop--];
    char op = calc->opStack[calc->opTop--];
    int result = 0;

    if (op == '+')
        result = first + second;
    else if (op == '-')
        result = first - second;
    else if (op == '*')
        result = first * second;
    else if (op == '/') {
        if (second == 0)
            return CALC_ERR_DIV_ZERO;
        result = first / second;
    }
    else
        return CALC_ERR_INVALID;

    return pushNumber(calc, result);
}

CalcStatus evaluate(const char *expr, int *result) {
    Calculator calc;
    int i, number, digit;
    int expectNumber = 1;
    CalcStatus status;

    initCalculator(&calc);

    for (i = 0; expr[i] != '\0' && expr[i] != '\n'; i++) {
        if (expr[i] == ' ' || expr[i] == '\t')
            continue;

        if (isdigit((unsigned char)expr[i])) {
            if (expectNumber == 0)
                return CALC_ERR_INVALID;

            number = 0;
            while (isdigit((unsigned char)expr[i])) {
                digit = expr[i] - '0';
                if (number > (INT_MAX - digit) / 10)
                    return CALC_ERR_INVALID;
                number = number * 10 + digit;
                i++;
            }
            i--;

            status = pushNumber(&calc, number);
            if (status != CALC_OK)
                return status;
            expectNumber = 0;
        }
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            if (expectNumber == 1)
                return CALC_ERR_INVALID;

            // if old operator is same or higher, solve it first
            while (calc.opTop >= 0 &&
                   precedence(calc.opStack[calc.opTop]) >= precedence(expr[i])) {
                status = applyOperator(&calc);
                if (status != CALC_OK)
                    return status;
            }

            status = pushOperator(&calc, expr[i]);
            if (status != CALC_OK)
                return status;
            expectNumber = 1;
        }
        else {
            return CALC_ERR_INVALID;
        }
    }

    if (expectNumber == 1)
        return CALC_ERR_INVALID;

    while (calc.opTop >= 0) {
        status = applyOperator(&calc);
        if (status != CALC_OK)
            return status;
    }

    *result = calc.numStack[calc.numTop];
    return CALC_OK;
}

int main() {
    char expr[MAX_INPUT_SIZE];
    int result;
    CalcStatus status;

    if (fgets(expr, sizeof(expr), stdin) == NULL) {
        printf("Error: Invalid expression.\n");
        return 1;
    }

    status = evaluate(expr, &result);

    if (status == CALC_OK)
        printf("%d\n", result);
    else if (status == CALC_ERR_DIV_ZERO)
        printf("Error: Division by zero.\n");
    else
        printf("Error: Invalid expression.\n");

    return 0;
}
