#include<stdio.h>
#include<stdlib.h>

typedef struct stack{
    int size;
    int top;
    int* arr;
}stack;

int is_empty(stack* ptr){//O(1)
    if(ptr->top==-1){
        return 1;
    }
    return 0;
}

int is_full(stack* ptr){//O(1)
    if(ptr->top==ptr->size-1){
        return 1;
    }
    return 0;
}

void  push(stack* ptr,int value){//O(1)
    if(is_full(ptr)){
        printf("Stack Overflow\n");
        return;
    }
    else{
        ptr->top++;
        ptr->arr[ptr->top]=value;
    }
}

int pop(stack* ptr){//O(1)
    if(is_empty(ptr)){
        printf("Stack Underflow");
        return -1;//assuming -1 never occurs in the stack
    }
    else{
        return(ptr->arr[ptr->top--]);
    }
}

int peak(stack*ptr, int i){//i refers to the position of the element we want to peak
    if(ptr->top-i+1<0){//O(1)
        printf("Invalid position");
        return -1;
    }
    else{
        return ptr->arr[ptr->top-i+1];
    }
}

int main(){
    stack *s;
    s=(stack*)malloc(sizeof(stack));
    s->size=5;
    s->top=-1;
    s->arr=(int*)malloc(sizeof(int)*(s->size));
    return 0;
    }
