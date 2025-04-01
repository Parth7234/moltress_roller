#include <stdio.h>
#include <stdlib.h>

#define MAX 3  // Maximum keys per node (order = MAX+1)

typedef struct BPlusTreeNode {
    int keys[MAX];  
    struct BPlusTreeNode *children[MAX + 1];  
    struct BPlusTreeNode *nextLeaf;  
    int numKeys;  
    int leaf;  
} BPlusTreeNode;

BPlusTreeNode *createNode(int leaf) {
    BPlusTreeNode *node = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    node->leaf = leaf;
    node->numKeys = 0;
    node->nextLeaf = NULL;
    for (int i = 0; i <= MAX; i++)
        node->children[i] = NULL;
    return node;
}

void traverse(BPlusTreeNode *root) {
    if (root == NULL)
        return;
    if (root->leaf) {
        for (int i = 0; i < root->numKeys; i++)
            printf("%d ", root->keys[i]);
        return;
    }
    for (int i = 0; i < root->numKeys; i++) {
        traverse(root->children[i]);
        printf("%d ", root->keys[i]);  
    }
    traverse(root->children[root->numKeys]);
}

BPlusTreeNode *search(BPlusTreeNode *root, int key) {
    if (root == NULL)
        return NULL;
    int i = 0;
    while (i < root->numKeys && key > root->keys[i])
        i++;
    if (root->leaf) {
        if (i < root->numKeys && root->keys[i] == key)
            return root;  
        return NULL;
    }
    return search(root->children[i], key);
}

void splitChild(BPlusTreeNode *parent, int i, BPlusTreeNode *child) {
    BPlusTreeNode *newNode = createNode(child->leaf);
    newNode->numKeys = MAX / 2;
    for (int j = 0; j < MAX / 2; j++)
        newNode->keys[j] = child->keys[j + MAX / 2];

    if (!child->leaf) {
        for (int j = 0; j <= MAX / 2; j++)
            newNode->children[j] = child->children[j + MAX / 2];
    } else {
        newNode->nextLeaf = child->nextLeaf;
        child->nextLeaf = newNode;
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

void insertNonFull(BPlusTreeNode *node, int key) {
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

void insert(BPlusTreeNode **root, int key) {
    if ((*root)->numKeys == MAX) {
        BPlusTreeNode *newRoot = createNode(0);
        newRoot->children[0] = *root;
        splitChild(newRoot, 0, *root);
        insertNonFull(newRoot, key);
        *root = newRoot;
    } else {
        insertNonFull(*root, key);
    }
}

// Delete a key from a leaf node
void removeKeyFromLeaf(BPlusTreeNode *node, int key) {
    int i = 0;
    while (i < node->numKeys && node->keys[i] != key)
        i++;

    if (i < node->numKeys) {  
        for (; i < node->numKeys - 1; i++)
            node->keys[i] = node->keys[i + 1];

        node->numKeys--;
    }
}

// Delete function
void deleteKey(BPlusTreeNode *node, int key) {
    int i = 0;
    while (i < node->numKeys && key > node->keys[i])
        i++;

    if (node->leaf) {
        removeKeyFromLeaf(node, key);
        return;
    }

    if (i < node->numKeys && node->keys[i] == key) {
        if (node->children[i]->numKeys > MAX / 2) {
            int pred = node->children[i]->keys[node->children[i]->numKeys - 1];
            node->keys[i] = pred;
            deleteKey(node->children[i], pred);
        } else if (node->children[i + 1]->numKeys > MAX / 2) {
            int succ = node->children[i + 1]->keys[0];
            node->keys[i] = succ;
            deleteKey(node->children[i + 1], succ);
        } else {
            for (int j = 0; j < node->children[i + 1]->numKeys; j++)
                node->children[i]->keys[node->children[i]->numKeys++] = node->children[i + 1]->keys[j];

            free(node->children[i + 1]);

            for (int j = i; j < node->numKeys - 1; j++)
                node->keys[j] = node->keys[j + 1];

            for (int j = i + 1; j <= node->numKeys; j++)
                node->children[j] = node->children[j + 1];

            node->numKeys--;
            deleteKey(node->children[i], key);
        }
    } else {
        deleteKey(node->children[i], key);
    }
}

int main() {
    BPlusTreeNode *root = createNode(1);

    insert(&root, 10);
    insert(&root, 20);
    insert(&root, 5);
    insert(&root, 6);
    insert(&root, 12);
    insert(&root, 30);
    insert(&root, 7);
    insert(&root, 17);

    printf("B+ Tree: ");
    traverse(root);
    printf("\n");

    printf("Searching for 12: %s\n", search(root, 12) ? "Found" : "Not Found");

    printf("Deleting 6...\n");
    deleteKey(root, 6);
    printf("B+ Tree after deletion: ");
    traverse(root);
    printf("\n");

    return 0;
}
