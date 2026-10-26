#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50
#define DEPT_LEN 30

/* ---------- Student Record Structure ---------- */
typedef struct Student {
    int  enrolID;                 /* BST key - unique */
    char name[NAME_LEN];
    char department[DEPT_LEN];
    struct Student *left;
    struct Student *right;
} Student;

/* Global comparison counter (used for complexity analysis) */
long comparisons = 0;

/* ---------- Utility : create a new node ---------- */
Student* createNode(int id, const char *name, const char *dept) {
    Student *node = (Student*)malloc(sizeof(Student));
    if (!node) { printf("Memory allocation failed!\n"); exit(1); }
    node->enrolID = id;
    strncpy(node->name, name, NAME_LEN - 1);
    node->name[NAME_LEN - 1] = '\0';
    strncpy(node->department, dept, DEPT_LEN - 1);
    node->department[DEPT_LEN - 1] = '\0';
    node->left = node->right = NULL;
    return node;
}

/* ---------- Insertion (rejects duplicate IDs) ---------- */
Student* insert(Student *root, int id, const char *name, const char *dept, int *status) {
    if (root == NULL) {
        *status = 1;             /* inserted */
        return createNode(id, name, dept);
    }
    comparisons++;
    if (id < root->enrolID)
        root->left = insert(root->left, id, name, dept, status);
    else if (id > root->enrolID)
        root->right = insert(root->right, id, name, dept, status);
    else {
        /* Duplicate ID */
        *status = 0;
    }
    return root;
}

/* ---------- Search by Enrolment ID ---------- */
Student* search(Student *root, int id) {
    if (root == NULL) return NULL;
    comparisons++;
    if (id == root->enrolID) return root;
    else if (id < root->enrolID) return search(root->left, id);
    else return search(root->right, id);
}

/* ---------- Find minimum node (used in deletion) ---------- */
Student* findMin(Student *root) {
    while (root && root->left) root = root->left;
    return root;
}

/* ---------- Delete node ---------- */
Student* deleteNode(Student *root, int id, int *found) {
    if (root == NULL) return NULL;

    if (id < root->enrolID)
        root->left = deleteNode(root->left, id, found);
    else if (id > root->enrolID)
        root->right = deleteNode(root->right, id, found);
    else {
        /* Found the node */
        *found = 1;

        /* Case 1: Leaf node */
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        /* Case 2: One child */
        else if (root->left == NULL) {
            Student *tmp = root->right;
            free(root);
            return tmp;
        }
        else if (root->right == NULL) {
            Student *tmp = root->left;
            free(root);
            return tmp;
        }
        /* Case 3: Two children - replace with inorder successor */
        else {
            Student *succ = findMin(root->right);
            root->enrolID = succ->enrolID;
            strcpy(root->name, succ->name);
            strcpy(root->department, succ->department);
            root->right = deleteNode(root->right, succ->enrolID, found);
        }
    }
    return root;
}

/* ---------- Traversals ---------- */
void inorder(Student *root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("  ID: %-5d | Name: %-20s | Dept: %s\n",
           root->enrolID, root->name, root->department);
    inorder(root->right);
}

void preorder(Student *root) {
    if (root == NULL) return;
    printf("  ID: %-5d | Name: %-20s | Dept: %s\n",
           root->enrolID, root->name, root->department);
    preorder(root->left);
    preorder(root->right);
}

void postorder(Student *root) {
    if (root == NULL) return;
    postorder(root->left);
    postorder(root->right);
    printf("  ID: %-5d | Name: %-20s | Dept: %s\n",
           root->enrolID, root->name, root->department);
}

/* ---------- Tree height (for complexity demonstration) ---------- */
int height(Student *root) {
    if (root == NULL) return 0;
    int lh = height(root->left);
    int rh = height(root->right);
    return 1 + (lh > rh ? lh : rh);
}

/* ---------- Free entire tree ---------- */
void freeTree(Student *root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* ---------- Load 10 sample balanced-looking records ---------- */
Student* loadSamples(Student *root) {
    int status;
    /* Order chosen to produce a reasonably balanced BST */
    int   ids[]   = {105, 102, 108, 101, 103, 107, 110, 104, 106, 109};
    char *names[] = {"Arjun","Bhavna","Chetan","Diya","Esha",
                     "Farah","Gaurav","Harsh","Isha","Jatin"};
    char *dept[]  = {"CSE","ECE","CSE","ME","CSE",
                     "IT","EEE","CSE","ECE","IT"};

    for (int i = 0; i < 10; i++)
        root = insert(root, ids[i], names[i], dept[i], &status);

    printf("\n>> 10 sample records inserted (balanced-order).\n");
    return root;
}

/* ---------- Complexity comparison demo ---------- */
void complexityDemo(void) {
    Student *balanced = NULL, *skewed = NULL;
    int status;

    int balancedOrder[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35};
    int sortedOrder[]   = {10, 20, 25, 30, 35, 40, 50, 60, 70, 80};

    /* Build balanced-looking BST */
    comparisons = 0;
    for (int i = 0; i < 10; i++)
        balanced = insert(balanced, balancedOrder[i], "Test", "Dept", &status);
    long compBalanced = comparisons;
    int  hBalanced    = height(balanced);

    /* Build skewed BST (sorted insertion) */
    comparisons = 0;
    for (int i = 0; i < 10; i++)
        skewed = insert(skewed, sortedOrder[i], "Test", "Dept", &status);
    long compSkewed = comparisons;
    int  hSkewed    = height(skewed);

    /* Search last element in both */
    comparisons = 0;
    search(balanced, 35);
    long searchBal = comparisons;

    comparisons = 0;
    search(skewed, 80);
    long searchSkew = comparisons;

    printf("\n============ Complexity Comparison ============\n");
    printf(" Balanced-order insertion:\n");
    printf("    Insertion comparisons : %ld\n", compBalanced);
    printf("    Tree height           : %d  (approx log2 n)\n", hBalanced);
    printf("    Search comparisons    : %ld\n", searchBal);

    printf("\n Sorted-order insertion (skewed):\n");
    printf("    Insertion comparisons : %ld\n", compSkewed);
    printf("    Tree height           : %d  (approx n)\n", hSkewed);
    printf("    Search comparisons    : %ld\n", searchSkew);

    printf("\n Conclusion:\n");
    printf("  - Average case BST operations : O(log n)\n");
    printf("  - Worst case  (skewed tree)   : O(n)\n");
    printf("  - Self-balancing trees (AVL / Red-Black) maintain\n");
    printf("    height = O(log n) via rotations after insert/delete,\n");
    printf("    guaranteeing O(log n) for all operations.\n");
    printf("================================================\n");

    freeTree(balanced);
    freeTree(skewed);
}

/* ---------- Menu ---------- */
void showMenu(void) {
    printf("\n========= Student Record Search System =========\n");
    printf(" 1. Insert new student record\n");
    printf(" 2. Search student by Enrolment ID\n");
    printf(" 3. Delete student by Enrolment ID\n");
    printf(" 4. Display records (Inorder - Sorted)\n");
    printf(" 5. Display records (Preorder)\n");
    printf(" 6. Display records (Postorder)\n");
    printf(" 7. Load 10 sample records\n");
    printf(" 8. Show tree height & reset counters\n");
    printf(" 9. Run complexity comparison demo\n");
    printf(" 0. Exit\n");
    printf("================================================\n");
    printf("Enter choice: ");
}

/* ---------- Main ---------- */
int main(void) {
    Student *root = NULL;
    int choice, id, status, found;
    char name[NAME_LEN], dept[DEPT_LEN];

    printf("** Student Record Search System using BST **\n");

    while (1) {
        showMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter Enrolment ID : ");
                scanf("%d", &id);
                printf("Enter Name         : ");
                scanf(" %[^\n]", name);
                printf("Enter Department   : ");
                scanf(" %[^\n]", dept);
                status = 0;
                root = insert(root, id, name, dept, &status);
                if (status) printf(">> Record inserted successfully.\n");
                else        printf(">> Duplicate Enrolment ID! Insertion rejected.\n");
                break;

            case 2:
                printf("Enter Enrolment ID to search: ");
                scanf("%d", &id);
                comparisons = 0;
                {
                    Student *s = search(root, id);
                    if (s) {
                        printf(">> Record Found:\n");
                        printf("   ID   : %d\n", s->enrolID);
                        printf("   Name : %s\n", s->name);
                        printf("   Dept : %s\n", s->department);
                    } else {
                        printf(">> Record NOT found.\n");
                    }
                    printf("   Comparisons used: %ld\n", comparisons);
                }
                break;

            case 3:
                printf("Enter Enrolment ID to delete: ");
                scanf("%d", &id);
                found = 0;
                root = deleteNode(root, id, &found);
                if (found) printf(">> Record with ID %d deleted.\n", id);
                else       printf(">> Record not found. Nothing deleted.\n");
                break;

            case 4:
                if (!root) { printf(">> Tree is empty.\n"); break; }
                printf("\n-- Inorder Traversal (Sorted by Enrolment ID) --\n");
                inorder(root);
                break;

            case 5:
                if (!root) { printf(">> Tree is empty.\n"); break; }
                printf("\n-- Preorder Traversal --\n");
                preorder(root);
                break;

            case 6:
                if (!root) { printf(">> Tree is empty.\n"); break; }
                printf("\n-- Postorder Traversal --\n");
                postorder(root);
                break;

            case 7:
                root = loadSamples(root);
                break;

            case 8:
                printf(">> Current tree height : %d\n", height(root));
                comparisons = 0;
                printf(">> Comparison counter reset to 0.\n");
                break;

            case 9:
                complexityDemo();
                break;

            case 0:
                freeTree(root);
                printf("Exiting... All memory freed. Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
