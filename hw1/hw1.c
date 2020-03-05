#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Products have sellers so
typedef struct Seller {
    char sellerName[32];
    int quantity;
    double price;
    struct Seller *next;
} SellerNode;

typedef struct ProductNode {
    char name[32];
    SellerNode *sellerList;
    struct ProductNode *next;
} ProductT;

typedef struct ShippingCost {
    char sellerName[32];
    double shippingCost;
    double minFreeShipping;
    struct ShippingCost *next;
} ShippingCostT;

void addProduct(char name[32], ProductT **list);

void addSellerForProduct(double price, char seller[11], char productName[32], ProductT **list,
                         ShippingCostT *shippingCostList);

void setShippingCost(char seller[11], double cost, double minFree, ShippingCostT **list);

int increaseProductInventory(char seller[11], char productName[32], int quantity, ProductT **list);

int purchaseProduct(char seller[11], char productName[32], int quantity, ProductT **list);

double getShippingCost(char seller[11], double productPrice, ShippingCostT *list);

void printProductList(char product[32], ProductT *products, ShippingCostT *shippingCostList);

int productExists(char product[32], ProductT *list);

void rightJustify(int valueLength, int spaceLength);

int main(int argc, char **args) {
    if (argc == 1) {
        printf("No input file specified");
        return 1;
    }
    ShippingCostT *shippingCostList = NULL;
    ProductT *productList = NULL;

    // Open the file
    FILE *inputFile = fopen(args[1], "r");

    // Read the first argument of each line until we reach the end of the file
    char *command = malloc(sizeof(char) * 20);
    while (fscanf(inputFile, "%s", command) != EOF) {
        // Map each command to each of the functions
        if (strcmp("DisplaySellerList", command) == 0) {
            char *product = malloc(sizeof(char) * 32);
            fscanf(inputFile, "%s", product);

            printProductList(product, productList, shippingCostList);
            free(product);
        } else {
            // All commands except DisplaySellerList have 3 arguments.
            // Reading them at the start and converting them as needed makes the code a lot smaller.
            char *argument1 = malloc(32);
            char *argument2 = malloc(32);
            char *argument3 = malloc(32);
            fscanf(inputFile, "%s %s %s", argument1, argument2, argument3);
            printf("%s %s %s %s", command, argument1, argument2, argument3);

            if (strcmp("SetShippingCost", command) == 0) {
                setShippingCost(argument1,
                                strtod(argument2, NULL),
                                strtod(argument3, NULL),
                                &shippingCostList);
            } else if (strcmp("SetProductPrice", command) == 0) {
                if (!productExists(argument1, productList)) {
                    addProduct(argument1, &productList);
                }
                addSellerForProduct(atof(argument3), argument2, argument1, &productList, shippingCostList);
            } else if (strcmp("IncreaseInventory", command) == 0) {
                printf(" %d", increaseProductInventory(argument2, argument1, atoi(argument3), &productList));
            } else if (strcmp("CustomerPurchase", command) == 0) {
                int inventory = purchaseProduct(argument2, argument1, atoi(argument3), &productList);
                if (inventory != -1) {
                    printf(" %d", inventory);
                } else {
                    printf(" NotEnoughInventoryError");
                }
            }

            printf("\n");
            free(argument1);
            free(argument2);
            free(argument3);
        }
    }
    fclose(inputFile);
    free(command);
    return 0;
}

// Adds a new product to productList given its name
void addProduct(char name[32], ProductT **list) {
    ProductT *product = (ProductT *) malloc(sizeof(ProductT));
    strcpy(product->name, name);
    product->sellerList = NULL;
    product->next = NULL;

    if (*list == NULL) {
        *list = product;
        return;
    } else if ((*list)->next == NULL) {
        (*list)->next = product;
        return;
    }
    ProductT *tempNode = *list;
    while (tempNode->next != NULL) {
        tempNode = (ProductT *) tempNode->next;
    }
    tempNode->next = product;
}

// Searches productList for the product then
void addSellerForProduct(double price, char seller[11], char productName[32], ProductT **list,
                         ShippingCostT *shippingCostList) {
    ProductT *tempNode = *list;
    while (tempNode != NULL) {
        if (strcmp(tempNode->name, productName) == 0) {
            // Create and initialize the newSeller node
            SellerNode *newSeller = (SellerNode *) malloc(sizeof(SellerNode));
            strcpy(newSeller->sellerName, seller);
            newSeller->quantity = 0;
            newSeller->price = price;
            newSeller->next = NULL;

            double newSellerShippingCost = getShippingCost(seller, price, shippingCostList);
            double newSellerTotal = price + newSellerShippingCost;
            // Initialize list
            if (tempNode->sellerList == NULL) {
                tempNode->sellerList = newSeller;
                break;
            }
            // Replace head if the new node is before it
            double headNodeTotal = tempNode->sellerList->price +
                                   getShippingCost(tempNode->sellerList->sellerName, tempNode->sellerList->price,
                                                   shippingCostList);
            if (newSellerTotal <= headNodeTotal) {
                // If they're equal compare the strings.
                // If the new seller is alphabetically after the current
                // head then add it after, otherwise add it before
                if(newSellerTotal == headNodeTotal){
                    int i;
                    for(i = 0; i < strlen(seller); i++){
                        if(seller[i] > tempNode->sellerList->sellerName[i]){
                            newSeller->next = tempNode->sellerList->next;
                            tempNode->sellerList->next = newSeller;
                            break;
                        }
                    }
                }
                newSeller->next = tempNode->sellerList;
                tempNode->sellerList = newSeller;
            }
            // Add the new node to head->next if NULL because the sorting algorithm below needs a list of at least 2
            else if (tempNode->sellerList->next == NULL) {
                tempNode->sellerList->next = newSeller;
            } else {
                SellerNode *tempSellerNode = tempNode->sellerList;
                SellerNode *nextSellerNode = tempNode->sellerList->next;

                // Keep track of the shippingCost of the node we are comparing to so we can sort by totalPrice
                double nextShippingCost = getShippingCost(nextSellerNode->sellerName, nextSellerNode->price,
                                                          shippingCostList);

                // Traverse the array until next_node is bigger/after the new_node then
                // add the new_node before it, putting the list in ascending order
                while (nextSellerNode != NULL &&
                       price + newSellerShippingCost > nextSellerNode->price + nextShippingCost) {
                    if (nextSellerNode->next != NULL) {
                        nextShippingCost = getShippingCost(nextSellerNode->sellerName, nextSellerNode->price,
                                                           shippingCostList);
                    }
                    tempSellerNode = tempSellerNode->next;
                    nextSellerNode = nextSellerNode->next;
                }
                newSeller->next = nextSellerNode;
                tempSellerNode->next = newSeller;
            }
            break;
        }
        tempNode = tempNode->next;
    }
}

// Search productList for the product and once its found, search the product's
// sellerList for the seller then increment its inventory
// Returns -1 if the quantity is negative or if it can't find the product
int increaseProductInventory(char seller[11], char productName[32], int quantity, ProductT **list) {
    if (quantity < 0) {
        return -1;
    }
    ProductT *tempNode = *list;
    while (tempNode != NULL) {
        if (strcmp(tempNode->name, productName) == 0) {
            SellerNode *tempSellerNode = tempNode->sellerList;
            while (tempSellerNode != NULL) {
                if (strcmp(seller, tempSellerNode->sellerName) == 0) {
                    tempSellerNode->quantity += quantity;
                    return tempSellerNode->quantity;
                }
                tempSellerNode = tempSellerNode->next;
            }
        }
        tempNode = tempNode->next;
    }
    return -1;
}

// This function adds a node to the ShippingCost list given the seller name, shippingCost, and a minimum for free shipping
void setShippingCost(char seller[11], double cost, double minFree, ShippingCostT **list) {
    ShippingCostT *shippingCost = (ShippingCostT *) malloc(sizeof(ShippingCostT));
    strcpy(shippingCost->sellerName, seller);
    shippingCost->shippingCost = cost;
    shippingCost->minFreeShipping = minFree;
    shippingCost->next = NULL;

    if (*list == NULL) {
        *list = shippingCost;
        return;
    } else if ((*list)->next == NULL) {
        (*list)->next = shippingCost;
        return;
    }

    ShippingCostT *tempNode = *list;
    while (tempNode->next != NULL) {
        tempNode = tempNode->next;
    }
    tempNode->next = shippingCost;
}

// Searches shippingCostList for a seller and determines if the productPrice is eligible for free shipping
double getShippingCost(char seller[11], double productPrice, ShippingCostT *list) {
    ShippingCostT *tempNode = list;
    while (tempNode != NULL) {
        if (strcmp(tempNode->sellerName, seller) == 0) {
            return productPrice < tempNode->minFreeShipping ? tempNode->shippingCost : 0.0;
        }
        tempNode = tempNode->next;
    }
    return 0.0;
}

void printProductList(char product[32], ProductT *products, ShippingCostT *shippingCostList) {
    ProductT *tempNode = products;
    while (tempNode != NULL) {
        if (strcmp(tempNode->name, product) == 0) {
            printf("DisplaySellerList %s\n", tempNode->name);
            printf("    seller  productPrice  shippingCost  totalCost\n");

            // For each product, print out each seller unless it doesn't have any inventory
            SellerNode *tempSellerNode = tempNode->sellerList;
            while (tempSellerNode != NULL) {
                if (tempSellerNode->quantity == 0) {
                    tempSellerNode = tempSellerNode->next;
                    continue;
                }
                // Right justify sellerName in a 10 wide space which is the size of the space
                // created by the word seller plus the 4 spaces before it
                rightJustify(strlen(tempSellerNode->sellerName), 10);
                printf("%s  ", tempSellerNode->sellerName);

                //Use snprintf to convert the double to a string so we can get its length in order to right justify it
                char priceString[10];
                snprintf(priceString, 10, "%.2lf", tempSellerNode->price);
                rightJustify(strlen(priceString), 12);
                printf("%s  ", priceString);

                double shippingCost = getShippingCost(tempSellerNode->sellerName, tempSellerNode->price,
                                                      shippingCostList);
                char shippingCostString[10];
                snprintf(shippingCostString, 10, "%.2lf", shippingCost);
                rightJustify(strlen(shippingCostString), 12);
                printf("%.2lf  ", shippingCost);

                double totalCost = shippingCost + tempSellerNode->price;
                char totalCostString[10];
                snprintf(totalCostString, 10, "%.2lf", totalCost);
                rightJustify(strlen(totalCostString), 9);
                printf("%s", totalCostString);

                printf("\n");
                tempSellerNode = tempSellerNode->next;
            }
        }
        tempNode = tempNode->next;
    }
}

// Finds a seller in a product's sellerList then decrements the quantity if there's sufficient inventory
// Returns -1 if the purchase can't be completed
int purchaseProduct(char seller[11], char productName[32], int quantity, ProductT **list) {
    ProductT *tempNode = *list;
    while (tempNode != NULL) {
        if (strcmp(tempNode->name, productName) == 0) {
            SellerNode *tempSellerNode = tempNode->sellerList;
            while (tempSellerNode != NULL) {
                if (strcmp(seller, tempSellerNode->sellerName) == 0) {
                    if (tempSellerNode->quantity - quantity >= 0) {
                        tempSellerNode->quantity -= quantity;
                        return tempSellerNode->quantity;
                    } else {
                        return -1;
                    }
                }
                tempSellerNode = tempSellerNode->next;
            }
        }
        tempNode = tempNode->next;
    }
}

// Checks if a given product exists
int productExists(char product[32], ProductT *list) {
    ProductT *tempNode = list;
    while (tempNode != NULL) {
        if (strcmp(tempNode->name, product) == 0) {
            return 1;
        }
        tempNode = tempNode->next;
    }
    return 0;
}

// Right justifies a value given its printed length and the size of the space it goes in
void rightJustify(int valueLength, int spaceLength) {
    int i;
    for (i = 0; i < spaceLength - valueLength; i++) {
        printf(" ");
    }
}