#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_NAME_LENGTH 100
#define MAX_INPUT_LENGTH 100
#define MIN_TICKETS 1
#define MAX_TICKETS 20
#define DISCOUNT_MIN_TICKETS 5
#define DISCOUNT_PERCENT 10

typedef enum {
    STATUS_OK = 0,
    STATUS_INPUT_ERROR,
    STATUS_MEMORY_ERROR,
    STATUS_INVALID_CATEGORY
} Status;

struct Category {
    const char *name;
    int price;
};

// Single source of truth for categories: index + 1 is the menu number
static const struct Category CATEGORIES[] = {
    {"Standard", 150},
    {"Premium",  250},
    {"Recliner", 400}
};

#define CATEGORY_COUNT ((int)(sizeof(CATEGORIES) / sizeof(CATEGORIES[0])))

struct Customer {
    char name[MAX_NAME_LENGTH];
    int ticketCount;
    int category;
};

// Read one line from stdin into buffer, without the trailing newline.
// Returns 1 on success, 0 on EOF/read error, -1 if the line was too long.
int readLine(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }

    size_t length = strcspn(buffer, "\n");

    if (buffer[length] != '\n' && !feof(stdin)) {
        // Line did not fit: discard the rest of it
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        return -1;
    }

    buffer[length] = '\0';
    return 1;
}

// Name must be exactly one word containing only letters
int isValidName(const char *name) {
    if (name[0] == '\0') {
        return 0;
    }

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i])) {
            return 0;
        }
    }

    return 1;
}

Status readCustomerName(struct Customer *customer) {
    char input[MAX_NAME_LENGTH];

    while (1) {
        printf("\nEnter the customer Name: ");

        int result = readLine(input, sizeof(input));

        if (result == 0) {
            fprintf(stderr, "\nError: could not read name.\n");
            return STATUS_INPUT_ERROR;
        }

        if (result == -1) {
            printf("Name is too long! Maximum %d characters.", MAX_NAME_LENGTH - 1);
            continue;
        }

        if (input[0] == '\0') {
            printf("Name cannot be empty!");
            continue;
        }

        if (!isValidName(input)) {
            printf("Name must be a single word with letters only (no spaces, digits or symbols)!");
            continue;
        }

        strcpy(customer->name, input);
        return STATUS_OK;
    }
}

// Read an integer in [min, max]; keeps asking until input is valid
Status readIntInRange(const char *prompt, int min, int max, int *value) {
    char input[MAX_INPUT_LENGTH];

    while (1) {
        printf("%s", prompt);

        int result = readLine(input, sizeof(input));

        if (result == 0) {
            fprintf(stderr, "\nError: could not read input.\n");
            return STATUS_INPUT_ERROR;
        }

        if (result == -1) {
            printf("Input is too long!");
            continue;
        }

        int number;
        char extra;

        if (sscanf(input, "%d %c", &number, &extra) != 1) {
            printf("Input must be a whole number only!");
            continue;
        }

        if (number < min || number > max) {
            printf("Value must lie in range %d - %d!", min, max);
            continue;
        }

        *value = number;
        return STATUS_OK;
    }
}

Status readTicketCount(struct Customer *customer) {
    return readIntInRange("\nEnter Number of Tickets: ",
                          MIN_TICKETS, MAX_TICKETS, &customer->ticketCount);
}

Status readTicketCategory(struct Customer *customer) {
    printf("\nTicket Categories:");
    for (int i = 0; i < CATEGORY_COUNT; i++) {
        printf("\n  %d. %-9s - Rs %d", i + 1, CATEGORIES[i].name, CATEGORIES[i].price);
    }

    return readIntInRange("\nEnter Ticket Category: ",
                          1, CATEGORY_COUNT, &customer->category);
}

// Look up category details; NULL if the category number is invalid
const struct Category *getCategory(int category) {
    if (category < 1 || category > CATEGORY_COUNT) {
        return NULL;
    }
    return &CATEGORIES[category - 1];
}

int calculateDiscountPercent(int ticketCount) {
    return (ticketCount >= DISCOUNT_MIN_TICKETS) ? DISCOUNT_PERCENT : 0;
}

Status printBill(const struct Customer *customer) {
    const struct Category *category = getCategory(customer->category);

    if (category == NULL) {
        fprintf(stderr, "\nError: invalid ticket category %d.\n", customer->category);
        return STATUS_INVALID_CATEGORY;
    }

    double subtotal = (double)category->price * customer->ticketCount;
    double discount = subtotal * calculateDiscountPercent(customer->ticketCount) / 100.0;

    printf("\n----- Bill Summary -----");
    printf("\nCustomer: %s", customer->name);
    printf("\nTicket Category: %s", category->name);
    printf("\nTickets: %d", customer->ticketCount);
    printf("\nSubtotal: %.2lf", subtotal);
    printf("\nDiscount: %.2lf", discount);
    printf("\nFinal Bill: %.2lf\n", subtotal - discount);

    return STATUS_OK;
}

int main(void) {
    struct Customer *customer = malloc(sizeof(*customer));

    if (customer == NULL) {
        fprintf(stderr, "Error: memory allocation failed!\n");
        return STATUS_MEMORY_ERROR;
    }

    Status status = readCustomerName(customer);

    if (status == STATUS_OK) {
        status = readTicketCount(customer);
    }

    if (status == STATUS_OK) {
        status = readTicketCategory(customer);
    }

    if (status == STATUS_OK) {
        status = printBill(customer);
    }

    free(customer);
    return status;
}
