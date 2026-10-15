#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

struct Customer {
    char name[100];
    int N_tickets;
    int ticket_category;
};

const int STANDARD = 150;
const int PREMIUM = 250;
const int RECLINER = 400;

// Read and validate customer name
int readCustomerName(struct Customer *s) {
    char input[100];

    if(fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error fetching name!");
        return 0;
    }

    input[strcspn(input, "\n")] = '\0';

    int flag = 0;

    for(int i = 0; input[i] != '\0'; i++) {
        if(!isalpha((unsigned char)input[i]) && !isspace((unsigned char)input[i])) {
            printf("\nName must contain only letters!");
            printf("\nEnter Name: ");
            return readCustomerName(s);
        }

        if(!isspace((unsigned char)input[i])) {
            flag = 1;
        }
    }

    if(!flag) {
        printf("\nName Cannot be empty!");
        printf("\nEnter Name: ");
        return readCustomerName(s);
    }

    strcpy(s->name, input);

    return 1;
}

// Read and validate ticket count
int readTicketCount(struct Customer *s) {
    int n;
    char input[100];
    char extra;

    if(fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error Taking input");
        return 0;
    }

    if(sscanf(input, "%d %c", &n, &extra) != 1) {
        printf("\nInput must be Number only!");
        printf("\nEnter Number of Tickets: ");
        return readTicketCount(s);
    }

    if(n < 1 || n > 20) {
        printf("\nNo. of Tickets must lie in range 1 - 20");
        printf("\nEnter Number of Tickets: ");
        return readTicketCount(s);
    }

    s->N_tickets = n;
    return 1;
}

// Read and validate ticket category
int readTicketCategory(struct Customer *s) {
    int n;
    char input[100];
    char extra;

    if(fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error Taking input!");
        return 0;
    }

    if(sscanf(input, "%d %c", &n, &extra) != 1) {
        printf("\nEnter only Number!");
        printf("\nEnter Ticket Category: ");
        return readTicketCategory(s);
    }

    if(n < 1 || n > 3) {
        printf("\nTicket Category Must lie between 1 - 3");
        printf("\nEnter Ticket Category: ");
        return readTicketCategory(s);
    }

    s->ticket_category = n;
    return 1;
}

// Return price by category
int calculateTicketPrice(struct Customer *s) {
    int category = s->ticket_category;

    switch(category) {
        case 1: return STANDARD;
        case 2: return PREMIUM;
        case 3: return RECLINER;
        default: return -1;
    }
}

// Calculate subtotal
double calculateSubtotal(struct Customer *s) {
    int num_tickets = s->N_tickets;
    int price = calculateTicketPrice(s);

    if(price == -1) {
        printf("Error while fetching ticket price!");
        return 0;
    }

    return price * num_tickets;
}

// Return discount percentage
int calculateDiscount(int ticketCount) {
    if(ticketCount >= 5) return 10;
    else return 0;
}

// Display bill summary
void printBill(struct Customer *c, double Subtotal, int discount) {
    printf("\nCustomer: %s", c->name);

    switch(c->ticket_category) {
        case 1: printf("\nTicket Category: Standard"); break;
        case 2: printf("\nTicket Category: Premium"); break;
        case 3: printf("\nTicket Category: Recliner"); break;
        default: printf("Error!!"); return;
    }

    printf("\nTickets: %d", c->N_tickets);
    printf("\nSubtotal: %0.2lf", Subtotal);
    printf("\nDiscount: %0.2lf", (Subtotal * discount) / 100);
    printf("\nFinal Bill: %0.2lf", Subtotal - (Subtotal * discount) / 100);
}

int main() {
    struct Customer *c = malloc(sizeof(*c));

    if(c == NULL) {
        printf("Memory Allocation Failed!");
        return 1;
    }

    printf("\nEnter the customer Name: ");
    int a = readCustomerName(c);

    if(!a) {
        free(c);
        return 1;
    }

    printf("\nEnter Number of Tickets: ");
    int d = readTicketCount(c);

    if(!d) {
        free(c);
        return 1;
    }

    printf("\nEnter Ticket Category: ");
    int b = readTicketCategory(c);

    if(!b) {
        free(c);
        return 1;
    }

    double Subtotal = calculateSubtotal(c);

    if(!Subtotal) {
        free(c);
        return 1;
    }

    int discount = calculateDiscount(c->N_tickets);

    printBill(c, Subtotal, discount);

    free(c);

    return 0;
}
