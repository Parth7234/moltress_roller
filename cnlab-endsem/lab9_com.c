#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROUTERS 10
#define INF 999999

typedef struct {
    int node;
    int cost;
} Edge;

typedef struct {
    int origin;
    int seq;
    int num_neighbors;
    Edge neighbors[MAX_ROUTERS];
    int valid; // 1 if LSA exists, 0 otherwise
} LSA;

typedef struct {
    int id;
    int num_neighbors;
    int neighbors[MAX_ROUTERS];
    LSA lsdb[MAX_ROUTERS]; // indexed by origin router ID
    int next_hop[MAX_ROUTERS];
    int dist[MAX_ROUTERS];
} Router;

typedef struct {
    int router_id;
    int sender_id;
    LSA lsa;
} QueueItem;

void refresh_neighbors(int N, int adj[MAX_ROUTERS][MAX_ROUTERS], Router routers[]) {
    for (int i = 0; i < N; i++) {
        routers[i].num_neighbors = 0;
        for (int j = 0; j < N; j++) {
            if (adj[i][j] > 0 && adj[i][j] != INF) {
                routers[i].neighbors[routers[i].num_neighbors++] = j;
            }
        }
    }
}

void display_topology(int N, int adj[MAX_ROUTERS][MAX_ROUTERS]) {
    printf("\n--- Network Topology ---\n");
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (adj[i][j] > 0 && adj[i][j] != INF) {
                printf("Router %d -- Router %d -- Cost: %d\n", i, j, adj[i][j]);
            }
        }
    }
}

void simulate_hello(int N, int adj[MAX_ROUTERS][MAX_ROUTERS], Router routers[]) {
    printf("\n--- HELLO Packet Exchange ---\n");
    for (int i = 0; i < N; i++) {
        routers[i].num_neighbors = 0;
        for (int j = 0; j < N; j++) {
            if (adj[i][j] > 0 && adj[i][j] != INF) {
                routers[i].neighbors[routers[i].num_neighbors++] = j;
                printf("Router %d sends HELLO to Router %d\n", i, j);
                printf("Router %d receives HELLO from Router %d\n", j, i);
            }
        }
    }

    printf("\n--- Discovered Neighbors ---\n");
    for (int i = 0; i < N; i++) {
        printf("Router %d: ", i);
        for (int j = 0; j < routers[i].num_neighbors; j++) {
            printf("%d%s", routers[i].neighbors[j], (j == routers[i].num_neighbors - 1) ? "" : ", ");
        }
        printf("\n");
    }
}

LSA build_lsa_for_router(int r, int N, int adj[MAX_ROUTERS][MAX_ROUTERS], int seq) {
    LSA lsa;
    lsa.origin = r;
    lsa.seq = seq;
    lsa.num_neighbors = 0;
    lsa.valid = 1;

    for (int j = 0; j < N; j++) {
        if (adj[r][j] > 0 && adj[r][j] != INF) {
            lsa.neighbors[lsa.num_neighbors].node = j;
            lsa.neighbors[lsa.num_neighbors].cost = adj[r][j];
            lsa.num_neighbors++;
        }
    }
    return lsa;
}

void print_lsa(LSA lsa) {
    printf("LSA from Router %d:\n", lsa.origin);
    printf("  Sequence Number: %d\n", lsa.seq);
    printf("  Neighbors: ");
    for (int k = 0; k < lsa.num_neighbors; k++) {
        printf("(%d,%d)%s", lsa.neighbors[k].node, lsa.neighbors[k].cost,
               (k == lsa.num_neighbors - 1) ? "" : ", ");
    }
    printf("\n");
}

void generate_initial_lsas(int N, int adj[MAX_ROUTERS][MAX_ROUTERS], int seq_nums[], LSA initial_lsas[]) {
    printf("\n--- Initial LSA Generation ---\n");
    for (int i = 0; i < N; i++) {
        initial_lsas[i] = build_lsa_for_router(i, N, adj, seq_nums[i]);
        print_lsa(initial_lsas[i]);
    }
}

void flood_lsa(int start_router_id, LSA lsa, Router routers[]) {
    QueueItem queue[1000];
    int head = 0, tail = 0;

    queue[tail].router_id = start_router_id;
    queue[tail].sender_id = -1;
    queue[tail].lsa = lsa;
    tail++;

    while (head < tail) {
        QueueItem current = queue[head++];
        Router *current_router = &routers[current.router_id];

        for (int i = 0; i < current_router->num_neighbors; i++) {
            int neighbor_id = current_router->neighbors[i];

            if (neighbor_id == current.sender_id) {
                continue;
            }

            Router *neighbor = &routers[neighbor_id];

            printf("Router %d forwards LSA (Origin %d, Seq %d) to Router %d\n",
                   current.router_id, current.lsa.origin, current.lsa.seq, neighbor_id);

            LSA *existing_lsa = &neighbor->lsdb[current.lsa.origin];
            if (!existing_lsa->valid || existing_lsa->seq < current.lsa.seq) {
                printf("Router %d accepts LSA from Origin %d\n", neighbor_id, current.lsa.origin);
                neighbor->lsdb[current.lsa.origin] = current.lsa;

                queue[tail].router_id = neighbor_id;
                queue[tail].sender_id = current.router_id;
                queue[tail].lsa = current.lsa;
                tail++;
            } else {
                printf("Router %d discards duplicate/older LSA from Origin %d\n", neighbor_id, current.lsa.origin);
            }
        }
    }
}

void simulate_flooding(int N, Router routers[], LSA initial_lsas[]) {
    printf("\n--- LSA Flooding ---\n");
    for (int i = 0; i < N; i++) {
        routers[i].lsdb[i] = initial_lsas[i];
        flood_lsa(i, initial_lsas[i], routers);
    }
}

void display_received_lsas(int N, Router routers[], int router_id) {
    if (router_id < 0 || router_id >= N) {
        printf("Invalid Router ID.\n");
        return;
    }
    printf("\nRouter %d's received LSA records:\n", router_id);
    for (int i = 0; i < N; i++) {
        if (routers[router_id].lsdb[i].valid) {
            LSA lsa = routers[router_id].lsdb[i];
            printf("Origin %d, Sequence %d: ", lsa.origin, lsa.seq);
            for (int k = 0; k < lsa.num_neighbors; k++) {
                printf("(%d,%d)%s", lsa.neighbors[k].node, lsa.neighbors[k].cost,
                       (k == lsa.num_neighbors - 1) ? "" : ", ");
            }
            printf("\n");
        }
    }
}

// Part B - Task 5: Compute Routing Table using Dijkstra on the router's LSDB
void compute_routing_table(int N, Router *router) {
    int graph[MAX_ROUTERS][MAX_ROUTERS];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }

    // Reconstruct graph from the router's LSDB
    for (int i = 0; i < N; i++) {
        if (router->lsdb[i].valid) {
            LSA lsa = router->lsdb[i];
            for (int k = 0; k < lsa.num_neighbors; k++) {
                int v = lsa.neighbors[k].node;
                int c = lsa.neighbors[k].cost;
                graph[lsa.origin][v] = c;
            }
        }
    }

    int src = router->id;
    int visited[MAX_ROUTERS] = {0};

    for (int i = 0; i < N; i++) {
        router->dist[i] = INF;
        router->next_hop[i] = -1;
    }
    router->dist[src] = 0;
    router->next_hop[src] = src;

    for (int count = 0; count < N; count++) {
        int u = -1, min_dist = INF;
        for (int i = 0; i < N; i++) {
            if (!visited[i] && router->dist[i] < min_dist) {
                min_dist = router->dist[i];
                u = i;
            }
        }

        if (u == -1 || min_dist == INF) break;
        visited[u] = 1;

        for (int v = 0; v < N; v++) {
            if (!visited[v] && graph[u][v] != INF && u != v) {
                int new_dist = router->dist[u] + graph[u][v];
                int candidate_nh = (u == src) ? v : router->next_hop[u];

                if (new_dist < router->dist[v]) {
                    router->dist[v] = new_dist;
                    router->next_hop[v] = candidate_nh;
                } else if (new_dist == router->dist[v]) {
                    // Tie-breaker: select route with smallest next-hop router ID
                    if (candidate_nh < router->next_hop[v]) {
                        router->next_hop[v] = candidate_nh;
                    }
                }
            }
        }
    }
}

void display_routing_table(int N, Router routers[], int router_id) {
    if (router_id < 0 || router_id >= N) {
        printf("Invalid Router ID.\n");
        return;
    }
    compute_routing_table(N, &routers[router_id]);
    printf("\nRouting Table for Router %d:\n", router_id);
    printf("%-12s | %-10s | %-10s\n", "Destination", "Next Hop", "Total Cost");
    printf("--------------------------------------\n");
    for (int i = 0; i < N; i++) {
        if (routers[router_id].dist[i] == INF) {
            printf("%-12d | %-10s | %-10s\n", i, "-", "INF");
        } else {
            printf("%-12d | %-10d | %-10d\n", i, routers[router_id].next_hop[i], routers[router_id].dist[i]);
        }
    }
}

// Part B - Task 6: Topology Change and Route Recalculation
void handle_topology_change(int N, int adj[MAX_ROUTERS][MAX_ROUTERS], Router routers[], int seq_nums[]) {
    int u, v, new_cost;
    printf("Enter Router1 Router2 and New Cost (Enter 0 or -1 to remove link): ");
    scanf("%d %d %d", &u, &v, &new_cost);

    if (u < 0 || u >= N || v < 0 || v >= N || u == v) {
        printf("Invalid router IDs.\n");
        return;
    }

    if (new_cost <= 0) {
        adj[u][v] = INF;
        adj[v][u] = INF;
        printf("\nLink between Router %d and Router %d removed.\n", u, v);
    } else {
        adj[u][v] = new_cost;
        adj[v][u] = new_cost;
        printf("\nLink cost between Router %d and Router %d updated to %d.\n", u, v, new_cost);
    }

    // Update neighbor lists for active links
    refresh_neighbors(N, adj, routers);

    // Affected routers generate updated LSAs with incremented sequence numbers
    seq_nums[u]++;
    seq_nums[v]++;
    LSA lsa_u = build_lsa_for_router(u, N, adj, seq_nums[u]);
    LSA lsa_v = build_lsa_for_router(v, N, adj, seq_nums[v]);

    printf("\n--- Updated LSA Generation ---\n");
    print_lsa(lsa_u);
    print_lsa(lsa_v);

    // Update own LSDB and flood updated LSAs
    printf("\n--- Flooding Updated LSAs ---\n");
    routers[u].lsdb[u] = lsa_u;
    flood_lsa(u, lsa_u, routers);

    routers[v].lsdb[v] = lsa_v;
    flood_lsa(v, lsa_v, routers);

    // Recompute routing tables at all routers
    for (int i = 0; i < N; i++) {
        compute_routing_table(N, &routers[i]);
    }
    printf("\nLSDBs and Routing Tables updated across all routers.\n");
}

int main() {
    int N, M;
    int adj[MAX_ROUTERS][MAX_ROUTERS];
    Router routers[MAX_ROUTERS];
    int seq_nums[MAX_ROUTERS];
    LSA initial_lsas[MAX_ROUTERS];
    int lsas_generated = 0;
    int hello_done = 0;

    printf("Number of Routers: ");
    scanf("%d", &N);
    printf("Number of Links: ");
    scanf("%d", &M);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            adj[i][j] = (i == j) ? 0 : INF;
        }
        seq_nums[i] = 1;
        routers[i].id = i;
        routers[i].num_neighbors = 0;
        for (int j = 0; j < MAX_ROUTERS; j++) {
            routers[i].lsdb[j].valid = 0;
        }
    }

    printf("Enter the links (Router1 Router2 Cost):\n");
    for (int i = 0; i < M; i++) {
        int u, v, c;
        scanf("%d %d %d", &u, &v, &c);
        adj[u][v] = c;
        adj[v][u] = c;
    }

    int choice;
    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. Display Network Topology\n");
        printf("2. Simulate Hello Packet Exchange\n");
        printf("3. Generate LSAs\n");
        printf("4. Simulate LSA Flooding\n");
        printf("5. Display Received LSAs at a Router\n");
        printf("6. Display Routing Table at a Router (Part B)\n");
        printf("7. Modify / Remove Link (Part B)\n");
        printf("8. Exit\n");
        printf("Select Option: ");
        scanf("%d", &choice);

        if (choice == 1) {
            display_topology(N, adj);
        } else if (choice == 2) {
            simulate_hello(N, adj, routers);
            hello_done = 1;
        } else if (choice == 3) {
            generate_initial_lsas(N, adj, seq_nums, initial_lsas);
            lsas_generated = 1;
        } else if (choice == 4) {
            if (!hello_done) {
                refresh_neighbors(N, adj, routers);
                hello_done = 1;
            }
            if (!lsas_generated) {
                generate_initial_lsas(N, adj, seq_nums, initial_lsas);
                lsas_generated = 1;
            }
            simulate_flooding(N, routers, initial_lsas);
        } else if (choice == 5) {
            int r;
            printf("Enter Router ID: ");
            scanf("%d", &r);
            display_received_lsas(N, routers, r);
        } else if (choice == 6) {
            int r;
            printf("Enter Router ID: ");
            scanf("%d", &r);
            display_routing_table(N, routers, r);
        } else if (choice == 7) {
            handle_topology_change(N, adj, routers, seq_nums);
        } else if (choice == 8) {
            break;
        } else {
            printf("Invalid Option.\n");
        }
    }

    return 0;
}