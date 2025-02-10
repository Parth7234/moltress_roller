#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Node{
    char name[50];
    char pn[10];
    char city[50];
    struct Node* next;
    struct Node* prev;
}Node;


void createdll(int n, Node** head, Node** last){
    Node* newnode;
    Node* temp;
    newnode=(Node*)malloc(sizeof(Node));
    newnode->prev=NULL;
    newnode->next=NULL;
    printf("Enter the name in node 1\n");
    scanf("%s",newnode->name);
    printf("Enter the phone number\n");
    scanf("%s",newnode->pn);
    printf("Enter the city\n");
    scanf("%s",newnode->city);
    temp=newnode;
    *head=newnode;
    int i;
    for(i=2;i<=n;i++){
        newnode=(Node*)malloc(sizeof(Node));
        printf("Enter the name in node %d\n",i);
        scanf("%s",newnode->name);
        printf("Enter the phone number\n");
        scanf("%s",newnode->pn);
        printf("Enter the city\n");
        scanf("%s",newnode->city);
        newnode->next=NULL;
        newnode->prev=temp;
        temp->next=newnode;

        temp=newnode;
    }
    *last=newnode;

    printf("Contact list created\n");
    temp=*head;
    for(i=1;i<=n;i++){
        printf("Name:%s, Phone:%s, City:%s\n",temp->name,temp->pn,temp->city);
        temp=temp->next;
    }
}

void ask(struct Node* a, struct Node* b){
    printf("Select phone number to delete(1 or 2)\n");
    printf("1. Name:%s, Phone:%s, City:%s\n",a->name,a->pn,a->city);
    printf("2. Name:%s, Phone:%s, City:%s\n",b->name,b->pn,b->city);
    int choice;
    scanf("%d",&choice);
    if(choice==1){
        strcpy(a->pn,"no");
    }
    else{
        strcpy(b->pn,"no");
    }
    printf("Entry deleted successfully\n");
}

void delete(struct Node** head,int n){
    int i,j;
    struct Node* temp=*head;
    struct Node* temper;
    for(i=1;i<=n;i++){
        temper=temp;
        for(j=i+1;j<=n;j++){
            temper=temper->next;
            if((!strcmp(temper->pn,temp->pn))&&(strcmp(temp->pn,"no"))){
                ask(temp,temper);
            }
        }
        temp=temp->next;
    }
    printf("Final Contact list\n");
    temp=*head;
    for(i=1;i<=n;i++){
        if((strcmp(temp->pn,"no"))){
        printf("Name:%s, Phone:%s, City:%s\n",temp->name,temp->pn,temp->city);}
        temp=temp->next;
    }
}

int main(){
    int choice,p=2,num;
    while(choice!=4){
    printf("Menu\n");
    printf("1. Create Contact List \n");
    printf("2. Delete Entries with Duplicate Phone Numbers  \n");
    printf("3. Merge Two Contact Lists \n");
    printf("4. Exit \n");
    printf("Enter your choice\n");
    scanf("%d",&choice);
    Node* head1;
    Node* last1;
    Node* head2;
    Node* last2;
    switch(choice){
        case 1:
        printf("Enter the number of contacts\n");
        int n;
        scanf("%d",&n);
        createdll(n,&head1,&last1);
        break;

        case 2:
        delete(&head1,n);
        break;

        case 3:
        printf("Enter the number of contact in List %d;",p++);
        scanf("%d",&num);
        createdll(num,&head2,&last2);
        last1->next=head2;
        head2->prev=last1;
        delete(&head1,num+n);
        break;
    }
}}