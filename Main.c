#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

typedef struct {
    int list[MAX];
    int count;
} BackpatchList;

typedef struct {
    BackpatchList* truelist;
    BackpatchList* falselist;
} BoolExpr;

char code[MAX][50];
int nextinstr = 0;

// --------- Utility Functions ---------

BackpatchList* createList(int index) {
    BackpatchList* list = (BackpatchList*)malloc(sizeof(BackpatchList));
    list->list[0] = index;
    list->count = 1;
    return list;
}

BackpatchList* merge(BackpatchList* l1, BackpatchList* l2) {
    for (int i = 0; i < l2->count; i++) {
        l1->list[l1->count++] = l2->list[i];
    }
    return l1;
}

void backpatch(BackpatchList* list, int target) {
    char buffer[10];
    sprintf(buffer, "%d", target);
    for (int i = 0; i < list->count; i++) {
        strcat(code[list->list[i]], buffer);
    }
}

// --------- Code Generation ---------

BoolExpr* generateBoolExpr(char* cond) {
    BoolExpr* be = (BoolExpr*)malloc(sizeof(BoolExpr));
    char temp[50];

    sprintf(temp, "if %s goto ", cond);
    strcpy(code[nextinstr], temp);
    be->truelist = createList(nextinstr++);

    strcpy(code[nextinstr], "goto ");
    be->falselist = createList(nextinstr++);

    return be;
}

void generateStatement(char* stmt) {
    strcpy(code[nextinstr++], stmt);
}

// --------- Main Program ---------

int main() {
    char condition[50];
    char trueStmt[50];
    char falseStmt[50];

    printf("Enter the condition (e.g., a < b): ");
    fgets(condition, sizeof(condition), stdin);
    condition[strcspn(condition, "\n")] = 0;  // Remove newline

    printf("Enter the statement for TRUE block: ");
    fgets(trueStmt, sizeof(trueStmt), stdin);
    trueStmt[strcspn(trueStmt, "\n")] = 0;

    printf("Enter the statement for FALSE block: ");
    fgets(falseStmt, sizeof(falseStmt), stdin);
    falseStmt[strcspn(falseStmt, "\n")] = 0;

    // Generate code
    BoolExpr* expr = generateBoolExpr(condition);

    int trueBlock = nextinstr;
    generateStatement(trueStmt);

    sprintf(code[nextinstr++], "goto ");
    BackpatchList* skipElse = createList(nextinstr - 1);

    int falseBlock = nextinstr;
    generateStatement(falseStmt);

    int afterIfElse = nextinstr;

    // Output before backpatching
    printf("\nGenerated Three-Address Code (Before Backpatching):\n");
    for (int i = 0; i < nextinstr; i++) {
        printf("%d: %s\n", i, code[i]);
    }

    // Backpatching
    backpatch(expr->truelist, trueBlock);
    backpatch(expr->falselist, falseBlock);
    backpatch(skipElse, afterIfElse);

    // Output after backpatching
    printf("\nGenerated Three-Address Code (After Backpatching):\n");
    for (int i = 0; i < nextinstr; i++) {
        printf("%d: %s\n", i, code[i]);
    }

    return 0;
}
