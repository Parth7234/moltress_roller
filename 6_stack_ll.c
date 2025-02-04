#include<stdio.h>
#include<stdlib.h>

typedef struct Node{
    int data;
    struct Node* next;
};


int is_empty(struct Node* top){
    if(top==NULL){
        return 1;
    }
    return 0;
}

int is_full(){
    struct Node* n=(struct Node*)malloc(sizeof(struct Node));
    if(n==NULL){
        return 1;
    }
    free(n);
    return 0;
}

struct Node* push(struct Node* top,int value){
    if(is_full()){
        printf("Stack Overflow\n");
        return top;
    }
    else{
        struct Node* n=(struct Node*)malloc(sizeof(struct Node));
        n->data=value;
        n->next=top;
        top=n;
        return top;
    }
}

int pop(struct Node** top){
    if(is_empty(*top)){
        printf("Stack Underflow");
        return -1;
    }
    else{
        struct Node* n=(*top);
        (*top)=(*top)->next;
        int x=n->data;
        free(n);
        return x;
    }
}

void ll_traversal(struct Node* header){
    struct Node* temp;
    temp=header;
    while((temp)!=NULL){
        printf("%d\n",temp->data);
        temp=temp->next;
    }
}

int peek(struct Node* top,int position){
    struct Node* ptr=top;
    int i;
    for(i=0;i<position-1&&ptr!=NULL;i++){
        ptr=ptr->next;
    }
    if(ptr!=NULL){
        return ptr->data;
    }
    else{
        return -1;
    }
}

int stack_top(struct Node* top){
    return top->data;
}

int main(){
    struct Node* top=NULL;
    top=push(top,78);
    top=push(top,8);
    top=push(top,7);
    ll_traversal(top);
    int element=pop(&top);
    printf("%d\n",element);
    printf("%d\n",peek(top,1));
}
