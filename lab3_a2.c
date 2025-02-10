#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct Node{
    float coeff;
    int exp;
    struct Node* next;
    struct Node* prev;
}Node;

void createdll(int n,Node** head,Node** last){
    Node* newnode;
    Node* temp;
    newnode=(Node*)malloc(sizeof(Node));
    newnode->prev=NULL;
    newnode->next=NULL;
    printf("Enter coefficient and exponent (space-separated) for term 1: \n");
    scanf("%f%d",&newnode->coeff,&newnode->exp);
    temp=newnode;
    *head=newnode;
    int i;
    for(i=2;i<=n;i++){
        newnode=(Node*)malloc(sizeof(Node));
        printf("Enter coefficient and exponent (space-separated) for term %d: \n",i);
        scanf("%f%d",&newnode->coeff,&newnode->exp);
        newnode->next=NULL;
        newnode->prev=temp;
        temp->next=newnode;

        temp=newnode;
    }
    *last=newnode;
    temp=*head;
    printf("Polynomial\n");
    for(i=1;i<=n;i++){
        printf("%f*x^%d+ ",temp->coeff,temp->exp);
        temp=temp->next;
    }
    printf("\n");
}

int main(){
    printf("Enter the number of terms in polynomial 1");
    int n;
    scanf("%d",&n);
    Node* head1;
    Node* last1;
    createdll(n,&head1,&last1);

    printf("Enter the number of terms in polynomial 2");
    int m;
    scanf("%d",&m);
    Node* head2;
    Node* last2;
    createdll(m,&head2,&last2);

    Node* temp1=head1;
    Node* temp2=head2;
    float coeffarr[100];
    int exparr[100];
    int i,j,k=0;
    printf("Intermediate Result (Before Combining Terms):\n");
    for(i=1;i<=n;i++){
        for(j=1;j<=m;j++){
            coeffarr[k]=temp1->coeff*temp2->coeff;
            exparr[k]=temp1->exp+temp2->exp;
            printf("%fx^%d+ ",coeffarr[k],exparr[k]);
            temp2=temp2->next;
            k++;
        }
        temp2=head2;
        temp1=temp1->next;
    }
    printf("\n");
    printf("Combining terms with the same exponent... \n  ");
    printf("Resultant Polynomial\n");
    float finalcoeffarr[100]={0};
    for(i=0;i<m*n;i++){
        finalcoeffarr[exparr[i]]+=coeffarr[i];
    }

    i=0;
    while(finalcoeffarr[i]!=0){
        printf("%f*x^%d+ ",finalcoeffarr[i],i);
        i++;
    }
    
}