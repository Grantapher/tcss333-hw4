/*
 * Grant Toepfer - HW4 - TCSS 333
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constants */
#define MAX_NUM_CUST 20
#define MAX_NUM_ITEMS 10
#define MAX_NAME_LEN 30
#define MAX_LINE_LEN 100

/* Structs */
typedef struct {
    char* item;
    int dollars; // (actual_cost * 100) to avoid floating point arithmatic
    int cents;
    int amount;
} Purchase;

typedef Purchase* Purchase_ptr;

typedef struct {
    char* name;
    int count;
    Purchase_ptr* items;
} Customer;

typedef Customer* Customer_ptr;

/* Functions */
void fail(char* str, int code){
    fprintf(stderr, "%s\n", str);
    exit(code);
}

void outputPurchase(FILE* out, Purchase_ptr purchase) {
    fprintf(out, "%s %d $%d.%02d\n", purchase->item, purchase->amount, purchase->dollars, purchase->cents);
}

void outputCustomer(FILE* out, Customer_ptr customer) {
    fprintf(out, "%s\n", customer->name);
    Purchase_ptr* purchases = customer->items;
    int i;
    for(i = 0; i < MAX_NUM_ITEMS; i++) {
        if(NULL == purchases[i]) break;
        else outputPurchase(out, purchases[i]);
    }
    fprintf(out, "\n");
}

void outputCustomersChrono(Customer_ptr* customers) {
    FILE* out = fopen("hw4time.txt", "w");
    
    int i;
    for (i = 0; i < MAX_NUM_CUST; i++) {
        if(NULL == customers[i]) break;
        else outputCustomer(out, customers[i]);
    }

    fclose(out);
}

Customer_ptr newCustomer(char* name) {
    Customer_ptr customer = malloc(sizeof(Customer));
    customer->name = malloc(sizeof(char) * MAX_NAME_LEN);
    strcpy(customer->name, name);
    customer->count = 0;
    customer->items = malloc(sizeof(Purchase_ptr) * MAX_NUM_ITEMS);
 
    int i;
    for (i = 0; i < MAX_NUM_ITEMS; i++) {
        customer->items[i] = NULL;
    }
    
    return customer;
}

int getCustomerIndex(Customer_ptr* customers, char* name) {
    int i;
    for(i = 0; i < MAX_NUM_CUST; i++) {
        if(NULL == customers[i]) {
            customers[i] = newCustomer(name);
            return i;
        } else if (0 == strcmp(name, customers[i]->name)) {
            return i;
        }
    }

    return -1;
}

Customer_ptr getCustomerByName(Customer_ptr* customers, char* name) {
    int index = getCustomerIndex(customers, name);
    return customers[index];
}

Purchase_ptr newPurchaseFull(char* item, int amount, int dollars, int cents) {
    Purchase_ptr purchase = malloc(sizeof(Purchase));
    purchase->item = malloc(sizeof(char) * MAX_NAME_LEN); 
    strcpy(purchase->item, item);
    purchase->amount = amount;
    purchase->dollars = dollars;
    purchase->cents = cents;
    return purchase;
}

Purchase_ptr newPurchase(char* item) {
    return newPurchaseFull(item, 0, 0, 0);
}

int getPurchaseIndex(Purchase_ptr* items, char* name) {
    int i;
    for(i = 0; i < MAX_NUM_ITEMS; i++) {
        if(NULL == items[i]) {
            items[i] = newPurchase(name);
            return i;
        } else if (0 == strcmp(name, items[i]->item)) {
            return i;
        }
    }

    return -1;
}

Purchase_ptr getPurchaseByItemName(Purchase_ptr* purchases, char* name) {
    int index = getPurchaseIndex(purchases, name);
    return purchases[index];
}

void addPurchaseToPurchase(Purchase_ptr* purchases, Purchase_ptr purchase) {
    Purchase_ptr existingPurchase = getPurchaseByItemName(purchases, purchase->item);
    existingPurchase->amount += purchase->amount;
    existingPurchase->dollars = purchase->dollars;
    existingPurchase->cents = purchase->cents;
    free(purchase);
}

void addPurchase(Customer_ptr* customers, char* name, char* item, int amount, int dollars, int cents) {
   Customer_ptr customer = getCustomerByName(customers, name);
   addPurchaseToPurchase(customer->items, newPurchaseFull(item, amount, dollars, cents));
   customer->count++;
}

void readInputFile(Customer_ptr* customers) {
    FILE* in = fopen("hw4input.txt", "r");
    if (NULL == in) fail("hw4input.txt failed to open!", 1);
    
    char line[MAX_LINE_LEN];
    while (NULL != fgets(line, MAX_LINE_LEN, in)) {
        char name[MAX_NAME_LEN], item[MAX_NAME_LEN];
        int amount, dollars, cents;
        sscanf(line, "%s %d %s $%d.%d\n", name, &amount, item, &dollars, &cents);
        
        addPurchase(customers, name, item, amount, dollars, cents); 
    }

    fclose(in);
}

int main(void) {
    Customer_ptr* customers = malloc(sizeof(Customer_ptr) * MAX_NUM_CUST);
    int i;
    for (i = 0; i < MAX_NUM_CUST; i++) {
        customers[i] = NULL;
    }

    readInputFile(customers);
    outputCustomersChrono(customers);

    free(customers);
    
    return 0;
}    
