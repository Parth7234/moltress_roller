#include<stdio.h>
#include<stdlib.h>

typedef struct Node{
    int data;
    struct Node* next;
    struct Node* prev;
}Node;

Node* head;
Node* last;

void createdll(int n){
    Node* newnode;
    Node* temp;
    newnode=(Node*)malloc(sizeof(Node));
    newnode->prev=NULL;
    newnode->next=NULL;
    printf("Enter the data in node 1\n");
    scanf("%d",&newnode->data);
    temp=newnode;
    head=newnode;
    int i;
    for(i=2;i<=n;i++){
        newnode=(Node*)malloc(sizeof(Node));
        printf("Enter the data in node %d\n",i);
        scanf("%d",&newnode->data);
        newnode->next=NULL;
        newnode->prev=temp;
        temp->next=newnode;

        temp=newnode;
    }
    last=newnode;
}

void traverse(Node* head){
    Node* temp;
    temp=head;
    while(temp!=NULL){
        printf("%d\n",temp->data);
        temp=temp->next;
    }
}

void insert(Node* head){
    Node* newnode;
    Node* temp;
    newnode=(Node*)malloc(sizeof(Node));
    printf("Enter the position at which you want to insert the node\n");
    int pos,i;
    scanf("%d",&pos);
    printf("Enter the data you want to insert");
    scanf("%d",&newnode->data);
    temp=head;
    for(i=1;i<=pos-1;i++){
        temp=temp->next;
    }
    newnode->next=temp->next;
    newnode->prev=temp;
    temp->next->prev = newnode;
    temp->next = newnode;
}

void swapper(Node* a, Node* b) {
    if (a == b) return;

    Node* aPrev = a->prev;
    Node* aNext = a->next;
    Node* bPrev = b->prev;
    Node* bNext = b->next;

    if (aNext == b) {  // Adjacent nodes (a before b)
        a->next = bNext;
        a->prev = b;
        b->next = a;
        b->prev = aPrev;
        if (aPrev) aPrev->next = b;
        if (bNext) bNext->prev = a;
    } else if (bNext == a) {  // Adjacent nodes (b before a)
        b->next = aNext;
        b->prev = a;
        a->next = b;
        a->prev = bPrev;
        if (bPrev) bPrev->next = a;
        if (aNext) aNext->prev = b;
    } else {  // Non-adjacent nodes
        if (aPrev) aPrev->next = b;
        if (aNext) aNext->prev = b;
        if (bPrev) bPrev->next = a;
        if (bNext) bNext->prev = a;

        a->next = bNext;
        a->prev = bPrev;
        b->next = aNext;
        b->prev = aPrev;
    }

    if (a == head) head = b;
    else if (b == head) head = a;

    if (a == last) last = b;
    else if (b == last) last = a;
}


void sort(Node* head,int n){
    Node* temp;
    Node* minnode;
    Node* start;
    int min,i,num=0;
    start=head;
    temp=head;
    min=temp->data;

    for(i=1;i<=n;i++){
    
    
    while(temp!=NULL){
        if(min>temp->data){
            min=temp->data;
            minnode=temp;
            num++;
        }
       temp=temp->next; 
    }
    if(num>0){
    swapper(minnode,start);}
    start=start->next;
    temp=start;
    min=temp->data;
    }
}



int main(){
    head=(Node*)malloc(sizeof(Node));
    last=(Node*)malloc(sizeof(Node));
    printf("Enter the number of nodes\n");
    int n;
    scanf("%d",&n);
    createdll(n);
    traverse(head);
    insert(head);
    traverse(head);
    sort(head,n);
    traverse(head);
}