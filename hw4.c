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
//Utility
void fail(char* str, int code) {
    fprintf(stderr, "%s\n", str);
    exit(code);
}

//Output
void outputPurchaseChrono(FILE* out, Purchase_ptr purchase) {
    fprintf(out, "%s %d $%d.%02d\n", purchase->item, purchase->amount, purchase->dollars, purchase->cents);
}

void outputCustomerChrono(FILE* out, Customer_ptr customer) {
    fprintf(out, "%s\n", customer->name);
    Purchase_ptr* purchases = customer->items;
    int i;
    for (i = 0; i < MAX_NUM_ITEMS; i++) {
        if (NULL == purchases[i]) break;
        else outputPurchaseChrono(out, purchases[i]);
    }
    fprintf(out, "\n");
}

void outputCustomersChrono(Customer_ptr* customers) {
    FILE* out = fopen("hw4time.txt", "w");

    int i;
    for (i = 0; i < MAX_NUM_CUST; i++) {
        if (NULL == customers[i]) break;
        else outputCustomerChrono(out, customers[i]);
    }

    fclose(out);
}

void outputPurchaseMoney(FILE* out, Purchase_ptr purchase) {
    int total = purchaseTotal(purchase);
    fprintf(out, "%s %d $%d.%02d, Item Value = $%d.%02d\n", purchase->item, purchase->amount, purchase->dollars,
        purchase->cents, total / 100, total % 100);
}

void outputCustomerMoney(FILE* out, Customer_ptr customer) {
    int total = customerTotal(customer);
    fprintf(out, "%s, Total Order = $%d.%02d\n", customer->name, total / 100, total % 100);
    Purchase_ptr* purchases = customer->items;
    int i;
    for (i = 0; i < MAX_NUM_ITEMS; i++) {
        if (NULL == purchases[i]) break;
        else outputPurchaseMoney(out, purchases[i]);
    }
    fprintf(out, "\n");
}

void outputCustomersMoney(Customer_ptr* customers) {
    FILE* out = fopen("hw4money.txt", "w");

    int i;
    for (i = 0; i < MAX_NUM_CUST; i++) {
        if (NULL == customers[i]) break;
        else outputCustomerMoney(out, customers[i]);
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
    for (i = 0; i < MAX_NUM_CUST; i++) {
        if (NULL == customers[i]) {
            customers[i] = newCustomer(name);
            return i;
        }
        else if (0 == strcmp(name, customers[i]->name)) {
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
    for (i = 0; i < MAX_NUM_ITEMS; i++) {
        if (NULL == items[i]) {
            items[i] = newPurchase(name);
            return i;
        }
        else if (0 == strcmp(name, items[i]->item)) {
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
    free(purchase->item);
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

int purchaseTotal(Purchase_ptr p) {
    return (100 * p->dollars + p->cents) * p->amount;
}

int comparePurchases(const void* e1, const void* e2) {
    Purchase_ptr p1 = *((Purchase_ptr*) e1);
    Purchase_ptr p2 = *((Purchase_ptr*) e2);

    return purchaseTotal(p2) - purchaseTotal(p1);
}

int customerTotal(Customer_ptr c) {
    int i, total = 0;
    for (i = 0; i < MAX_NUM_ITEMS; i++) {
        if (NULL == c->items[i]) break;
        total += purchaseTotal(c->items[i]);
    }
    return total;
}

int compareCustomers(const void* e1, const void* e2) {
    Customer_ptr c1 = *((Customer_ptr*) e1);
    Customer_ptr c2 = *((Customer_ptr*) e2);

    return customerTotal(c2) - customerTotal(c1);
}

void swap(void* p1, void* p2, int size) {
    char* a1 = (char*) p1;
    char* a2 = (char*) p2;
    int i;

    for (i = 0; i < size; i++) {
        char temp = a1[i];
        a1[i] = a2[i];
        a2[i] = temp;
    }
}

void insertionSort(void* a, int n, int size, int(*cmp) (const void *, const void *)) {
    int i, j;
    char* array = (char*) a;
    for (i = 0; i < n; i++) {
        for (j = i; 0 < j && 0 < cmp(array + (j - 1) * size, array + j * size); j--) {
            swap(array + j * size, array + (j - 1) * size, size);
        }
    }
}

void sortCustomers(Customer_ptr* customers) {
    int i, j;
    for (i = 0; i < MAX_NUM_CUST; i++) {
        if (NULL == customers[i]) break;
        for (j = 0; j < MAX_NUM_ITEMS && NULL != customers[i]->items[j]; j++);
        insertionSort(customers[i]->items, j, sizeof(Purchase_ptr), comparePurchases);
    }
    insertionSort(customers, i, sizeof(Customer_ptr), compareCustomers);
}

void destructCustomers(Customer_ptr* customers) {
    int i, j;
    for (i = 0; i < MAX_NUM_CUST && NULL != customers[i]; i++) {
        free(customers[i]->name);
        for (j = 0; j < MAX_NUM_ITEMS && NULL != customers[i]->items[j]; j++) {
            free(customers[i]->items[j]->item);
            free(customers[i]->items[j]);
        }
        free(customers[i]->items);
        free(customers[i]);
    }
    free(customers);
}

int main(void) {
    Customer_ptr* customers = malloc(sizeof(Customer_ptr) * MAX_NUM_CUST);
    int i;
    for (i = 0; i < MAX_NUM_CUST; i++) {
        customers[i] = NULL;
    }

    readInputFile(customers);
    outputCustomersChrono(customers);
    sortCustomers(customers);
    outputCustomersMoney(customers);

    destructCustomers(customers);

    return 0;
}
