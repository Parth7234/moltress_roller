#include <stdio.h>
#include <stdlib.h>

#define MAX 3 // Maximum keys per node (Order of B-Tree is t=MAX+1)

typedef struct BTreeNode {
    int keys[MAX];
    struct BTreeNode *children[MAX + 1];
    int numKeys;
    int leaf;
} BTreeNode;

BTreeNode *createNode(int leaf) {
    BTreeNode *node = (BTreeNode *)malloc(sizeof(BTreeNode));
    node->leaf = leaf;
    node->numKeys = 0;
    for (int i = 0; i <= MAX; i++)
        node->children[i] = NULL;
    return node;
}

void traverse(BTreeNode *root) {
    if (root != NULL) {
        for (int i = 0; i < root->numKeys; i++) {
            if (!root->leaf)
                traverse(root->children[i]);
            printf("%d ", root->keys[i]);
        }
        if (!root->leaf)
            traverse(root->children[root->numKeys]);
    }
}

// Search function
BTreeNode *search(BTreeNode *root, int key) {
    int i = 0;
    while (i < root->numKeys && key > root->keys[i])
        i++;

    if (root->keys[i] == key)
        return root;

    if (root->leaf)
        return NULL;

    return search(root->children[i], key);
}

// Splitting a full child node
void splitChild(BTreeNode *parent, int i, BTreeNode *child) {
    BTreeNode *newNode = createNode(child->leaf);
    newNode->numKeys = MAX / 2;

    for (int j = 0; j < MAX / 2; j++)
        newNode->keys[j] = child->keys[j + MAX / 2 + 1];

    if (!child->leaf) {
        for (int j = 0; j <= MAX / 2; j++)
            newNode->children[j] = child->children[j + MAX / 2 + 1];
    }

    child->numKeys = MAX / 2;

    for (int j = parent->numKeys; j > i; j--)
        parent->children[j + 1] = parent->children[j];

    parent->children[i + 1] = newNode;

    for (int j = parent->numKeys - 1; j >= i; j--)
        parent->keys[j + 1] = parent->keys[j];

    parent->keys[i] = child->keys[MAX / 2];
    parent->numKeys++;
}

// Insert function
void insertNonFull(BTreeNode *node, int key) {
    int i = node->numKeys - 1;

    if (node->leaf) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->numKeys++;
    } else {
        while (i >= 0 && key < node->keys[i])
            i--;

        if (node->children[i + 1]->numKeys == MAX) {
            splitChild(node, i + 1, node->children[i + 1]);
            if (key > node->keys[i + 1])
                i++;
        }
        insertNonFull(node->children[i + 1], key);
    }
}

// Insert a new key
void insert(BTreeNode **root, int key) {
    if ((*root)->numKeys == MAX) {
        BTreeNode *newRoot = createNode(0);
        newRoot->children[0] = *root;
        splitChild(newRoot, 0, *root);
        insertNonFull(newRoot, key);
        *root = newRoot;
    } else {
        insertNonFull(*root, key);
    }
}

// Find the smallest key in a subtree
int getMin(BTreeNode *node) {
    while (!node->leaf)
        node = node->children[0];
    return node->keys[0];
}

// Remove a key from a node
void removeKey(BTreeNode *node, int key) {
    int i = 0;
    while (i < node->numKeys && key > node->keys[i])
        i++;

    if (node->leaf) {
        if (i < node->numKeys && node->keys[i] == key) {
            for (int j = i; j < node->numKeys - 1; j++)
                node->keys[j] = node->keys[j + 1];
            node->numKeys--;
        }
    } else {
        if (i < node->numKeys && node->keys[i] == key) {
            if (node->children[i]->numKeys > MAX / 2) {
                int pred = getMin(node->children[i]);
                node->keys[i] = pred;
                removeKey(node->children[i], pred);
            } else if (node->children[i + 1]->numKeys > MAX / 2) {
                int succ = getMin(node->children[i + 1]);
                node->keys[i] = succ;
                removeKey(node->children[i + 1], succ);
            } else {
                node->children[i]->keys[node->children[i]->numKeys++] = key;
                for (int j = 0; j < node->children[i + 1]->numKeys; j++)
                    node->children[i]->keys[node->children[i]->numKeys++] = node->children[i + 1]->keys[j];

                for (int j = i; j < node->numKeys - 1; j++)
                    node->keys[j] = node->keys[j + 1];

                for (int j = i + 1; j < node->numKeys; j++)
                    node->children[j] = node->children[j + 1];

                node->numKeys--;
                free(node->children[i + 1]);
                removeKey(node->children[i], key);
            }
        } else {
            removeKey(node->children[i], key);
        }
    }
}

// Delete a key from the tree
void deleteKey(BTreeNode **root, int key) {
    removeKey(*root, key);
    if ((*root)->numKeys == 0) {
        BTreeNode *oldRoot = *root;
        *root = (*root)->children[0];
        free(oldRoot);
    }
}

// Driver Code
int main() {
    BTreeNode *root = createNode(1);
    
    insert(&root, 10);
    insert(&root, 20);
    insert(&root, 5);
    insert(&root, 6);
    insert(&root, 12);
    insert(&root, 30);
    insert(&root, 7);
    insert(&root, 17);

    printf("B-Tree: ");
    traverse(root);
    printf("\n");

    printf("Searching for 12: %s\n", search(root, 12) ? "Found" : "Not Found");

    printf("Deleting 6...\n");
    deleteKey(&root, 6);
    printf("B-Tree: ");
    traverse(root);
    printf("\n");

    return 0;
}
