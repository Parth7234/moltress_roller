#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 100
#define INF INT_MAX

int graph[MAX][MAX]; // adjacency matrix

int findMinDistance(int dist[], int visited[], int n) {
    int min = INF, minIndex = -1;
    for (int i = 0; i < n; i++) {
        if (!visited[i] && dist[i] < min) {
            min = dist[i];
            minIndex = i;
        }
    }
    return minIndex;
}

void dijkstra(int n, int src) {
    int dist[MAX], visited[MAX];

    // Initialize distances and visited array
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }

    dist[src] = 0;

    for (int count = 0; count < n - 1; count++) {
        int u = findMinDistance(dist, visited, n);
        if (u == -1) break;

        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }

    // Print the shortest distances
    printf("Vertex \t Distance from Source %d\n", src);
    for (int i = 0; i < n; i++) {
        if (dist[i] == INF)
            printf("%d \t INF\n", i);
        else
            printf("%d \t %d\n", i, dist[i]);
    }
}

int main() {
    int n, edges, directed;
    printf("Enter number of vertices: ");
    scanf("%d", &n);
    printf("Enter number of edges: ");
    scanf("%d", &edges);
    printf("Is the graph directed? (1 for Yes, 0 for No): ");
    scanf("%d", &directed);

    // Initialize graph
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            graph[i][j] = 0;

    // Input edges and weights
    for (int i = 0; i < edges; i++) {
        int u, v, w;
        printf("Enter edge (from to weight): ");
        scanf("%d %d %d", &u, &v, &w);
        graph[u][v] = w;
        if (!directed)
            graph[v][u] = w;
    }

    int src;
    printf("Enter source vertex: ");
    scanf("%d", &src);

    dijkstra(n, src);

    return 0;
}
