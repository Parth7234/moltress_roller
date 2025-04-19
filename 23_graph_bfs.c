#include<stdio.h>
#include<stdlib.h>

int graph[100][100];

struct queue
{
    int size;
    int f;
    int r;
    int* arr;
};


 
 
int isEmpty(struct queue *q){
    if(q->r==q->f){
        return 1;
    }
    return 0;
}
 
int isFull(struct queue *q){
    if(q->r==q->size-1){
        return 1;
    }
    return 0;
}
 
void enqueue(struct queue *q, int val){
    if(isFull(q)){
        printf("This Queue is full\n");
    }
    else{
        q->r++;
        q->arr[q->r] = val;
    }
}
 
int dequeue(struct queue *q){
    int a = -1;
    if(isEmpty(q)){
        printf("This Queue is empty\n");
    }
    else{
        q->f++;
        a = q->arr[q->f]; 
    }
    return a;
}

int main(){
    struct queue q;
    q.size = 100;
    q.f = q.r = 0;
    q.arr = (int*) malloc(q.size*sizeof(int));
    int n;
    printf("Enter the number of vertices in the graph\n");
    scanf("%d",&n);
    printf("Enter number of edges in the graph\n");
    int edge;
    scanf("%d",&edge);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            graph[i][j]=0;
        }
    }
    
    for(int i=0;i<edge;i++){
        printf("Enter a edge pair\n");
        int x,y;
        scanf("%d%d",&x,&y);
        graph[x][y]=1;
        graph[y][x]=1;
    }
    
    int* visited=(int*)malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
        visited[i]=0;
    }
    int i=0;
    printf("%d ",i);
    visited[i]=1;
    enqueue(&q,i);
    while(!isEmpty(&q)){//code for bfs taversal
        int node=dequeue(&q);
        for(int j=0;j<n;j++){
            if(graph[node][j]==1&&visited[j]==0){
                printf("%d ",j);
                visited[j]=1;
                enqueue(&q,j);
            }
            
        }
        
    }
    printf("\n");
    //code for printing adjacency matrix

    for(int i=0;i<n;i++){
        printf("%d :",i);
        for(int j=0;j<n;j++){
            if(graph[i][j]==1){
                printf("%d ",j);
            }
        }
        printf("\n");
    }

    //some code for checking for cycles in a graph using bfs

//     #include <stdio.h>
// #include <stdlib.h>

// int graph[100][100];

// struct queue {
//     int size;
//     int f;
//     int r;
//     int* arr;
// };

// int isEmpty(struct queue *q) {
//     return q->r == q->f;
// }

// int isFull(struct queue *q) {
//     return q->r == q->size - 1;
// }

// void enqueue(struct queue *q, int val) {
//     if (isFull(q)) {
//         printf("Queue is full\n");
//     } else {
//         q->r++;
//         q->arr[q->r] = val;
//     }
// }

// int dequeue(struct queue *q) {
//     if (isEmpty(q)) {
//         printf("Queue is empty\n");
//         return -1;
//     } else {
//         q->f++;
//         return q->arr[q->f];
//     }
// }

// int bfsCycleCheck(int start, int n, int* visited, int* parent) {
//     struct queue q;
//     q.size = 100;
//     q.f = q.r = 0;
//     q.arr = (int*) malloc(q.size * sizeof(int));

//     visited[start] = 1;
//     parent[start] = -1;
//     enqueue(&q, start);

//     while (!isEmpty(&q)) {
//         int node = dequeue(&q);
//         for (int j = 0; j < n; j++) {
//             if (graph[node][j] == 1) {
//                 if (!visited[j]) {
//                     visited[j] = 1;
//                     parent[j] = node;
//                     enqueue(&q, j);
//                 } else if (parent[node] != j) {
//                     // Found a back edge (cycle)
//                     free(q.arr);
//                     return 1;
//                 }
//             }
//         }
//     }

//     free(q.arr);
//     return 0;
// }

// int main() {
//     int n, edge;
//     printf("Enter the number of vertices in the graph\n");
//     scanf("%d", &n);
//     printf("Enter number of edges in the graph\n");
//     scanf("%d", &edge);

//     // Initialize adjacency matrix
//     for (int i = 0; i < n; i++)
//         for (int j = 0; j < n; j++)
//             graph[i][j] = 0;

//     // Build the graph
//     for (int i = 0; i < edge; i++) {
//         int x, y;
//         printf("Enter an edge pair\n");
//         scanf("%d %d", &x, &y);
//         graph[x][y] = 1;
//         graph[y][x] = 1;  // Undirected graph
//     }

//     // Print adjacency list from matrix
//     printf("\nAdjacency List:\n");
//     for (int i = 0; i < n; i++) {
//         printf("%d :", i);
//         for (int j = 0; j < n; j++) {
//             if (graph[i][j] == 1)
//                 printf("%d ", j);
//         }
//         printf("\n");
//     }

//     // Cycle detection using BFS
//     int* visited = (int*)calloc(n, sizeof(int));
//     int* parent = (int*)malloc(n * sizeof(int));
//     int cycleFound = 0;

//     for (int i = 0; i < n; i++) {
//         if (!visited[i]) {
//             if (bfsCycleCheck(i, n, visited, parent)) {
//                 cycleFound = 1;
//                 break;
//             }
//         }
//     }

//     if (cycleFound)
//         printf("\nThe road map contains a cycle (circular route).\n");
//     else
//         printf("\nThe road map does NOT contain any cycle (no circular route).\n");

//     printf("\nProgram exited gracefully.\n");
//     free(visited);
//     free(parent);
//     return 0;
// }


}
