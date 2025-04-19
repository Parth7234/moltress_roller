#include<stdio.h>
#include<stdlib.h>

int graph[100][100];


void DFS( int i,int n,int* visited){
    printf("%d ",i);
    visited[i]=1;
    for(int j=0;j<n;j++){
        if(graph[i][j]==1&&!visited[j]){
            DFS(j,n,visited);
        }
        
    }
    return;
}

int main(){
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
 DFS(0,n,visited);
}
