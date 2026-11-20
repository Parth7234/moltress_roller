#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 10
#define INF 999999

typedef struct {
    int v, w;
} Edge;

typedef struct {
    int orig, seq, deg, ok;
    Edge nb[MAXN];
} LSA;

typedef struct {
    int id, deg;
    int nb[MAXN];
    LSA db[MAXN];
    int nh[MAXN];
    int dist[MAXN];
} Router;

typedef struct {
    int rid, from;
    LSA lsa;
} QNode;

void sync_nb(int n, int adj[MAXN][MAXN], Router rt[]) {
    for (int i = 0; i < n; i++) {
        rt[i].deg = 0;
        for (int j = 0; j < n; j++) {
            if (adj[i][j] > 0 && adj[i][j] != INF) {
                rt[i].nb[rt[i].deg++] = j;
            }
        }
    }
}

void show_topo(int n, int adj[MAXN][MAXN]) {
    printf("\n--- Network Topology ---\n");
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (adj[i][j] > 0 && adj[i][j] != INF) {
                printf("Router %d -- Router %d -- Cost: %d\n", i, j, adj[i][j]);
            }
        }
    }
}

void sim_hello(int n, int adj[MAXN][MAXN], Router rt[]) {
    printf("\n--- HELLO Packet Exchange ---\n");
    for (int i = 0; i < n; i++) {
        rt[i].deg = 0;
        for (int j = 0; j < n; j++) {
            if (adj[i][j] > 0 && adj[i][j] != INF) {
                rt[i].nb[rt[i].deg++] = j;
                printf("Router %d sends HELLO to Router %d\n", i, j);
                printf("Router %d receives HELLO from Router %d\n", j, i);
            }
        }
    }

    printf("\n--- Discovered Neighbors ---\n");
    for (int i = 0; i < n; i++) {
        printf("Router %d: ", i);
        for (int j = 0; j < rt[i].deg; j++) {
            printf("%d%s", rt[i].nb[j], (j == rt[i].deg - 1) ? "" : ", ");
        }
        printf("\n");
    }
}

LSA make_lsa(int r, int n, int adj[MAXN][MAXN], int seq) {
    LSA x;
    x.orig = r;
    x.seq = seq;
    x.deg = 0;
    x.ok = 1;

    for (int j = 0; j < n; j++) {
        if (adj[r][j] > 0 && adj[r][j] != INF) {
            x.nb[x.deg].v = j;
            x.nb[x.deg].w = adj[r][j];
            x.deg++;
        }
    }
    return x;
}

void show_lsa(LSA x) {
    printf("LSA from Router %d:\n", x.orig);
    printf("  Sequence Number: %d\n", x.seq);
    printf("  Neighbors: ");
    for (int k = 0; k < x.deg; k++) {
        printf("(%d,%d)%s", x.nb[k].v, x.nb[k].w, (k == x.deg - 1) ? "" : ", ");
    }
    printf("\n");
}

void gen_lsas(int n, int adj[MAXN][MAXN], int seq[], LSA lsas[]) {
    printf("\n--- Initial LSA Generation ---\n");
    for (int i = 0; i < n; i++) {
        lsas[i] = make_lsa(i, n, adj, seq[i]);
        show_lsa(lsas[i]);
    }
}

void flood(int src, LSA lsa, Router rt[]) {
    QNode q[1000];
    int f = 0, r = 0;

    q[r].rid = src;
    q[r].from = -1;
    q[r].lsa = lsa;
    r++;

    while (f < r) {
        QNode cur = q[f++];
        Router *u = &rt[cur.rid];

        for (int i = 0; i < u->deg; i++) {
            int nid = u->nb[i];
            if (nid == cur.from) continue;

            Router *v = &rt[nid];
            printf("Router %d forwards LSA (Origin %d, Seq %d) to Router %d\n",
                   cur.rid, cur.lsa.orig, cur.lsa.seq, nid);

            LSA *old = &v->db[cur.lsa.orig];
            if (!old->ok || old->seq < cur.lsa.seq) {
                printf("Router %d accepts LSA from Origin %d\n", nid, cur.lsa.orig);
                v->db[cur.lsa.orig] = cur.lsa;

                q[r].rid = nid;
                q[r].from = cur.rid;
                q[r].lsa = cur.lsa;
                r++;
            } else {
                printf("Router %d discards duplicate/older LSA from Origin %d\n", nid, cur.lsa.orig);
            }
        }
    }
}

void sim_flood(int n, Router rt[], LSA lsas[]) {
    printf("\n--- LSA Flooding ---\n");
    for (int i = 0; i < n; i++) {
        rt[i].db[i] = lsas[i];
        flood(i, lsas[i], rt);
    }
}

void show_db(int n, Router rt[], int r) {
    if (r < 0 || r >= n) {
        printf("Invalid Router ID.\n");
        return;
    }
    printf("\nRouter %d's received LSA records:\n", r);
    for (int i = 0; i < n; i++) {
        if (rt[r].db[i].ok) {
            LSA x = rt[r].db[i];
            printf("Origin %d, Sequence %d: ", x.orig, x.seq);
            for (int k = 0; k < x.deg; k++) {
                printf("(%d,%d)%s", x.nb[k].v, x.nb[k].w, (k == x.deg - 1) ? "" : ", ");
            }
            printf("\n");
        }
    }
}

void calc_rt(int n, Router *rt) {
    int g[MAXN][MAXN];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            g[i][j] = (i == j) ? 0 : INF;
        }
    }

    for (int i = 0; i < n; i++) {
        if (rt->db[i].ok) {
            LSA x = rt->db[i];
            for (int k = 0; k < x.deg; k++) {
                g[x.orig][x.nb[k].v] = x.nb[k].w;
            }
        }
    }

    int s = rt->id;
    int vis[MAXN] = {0};

    for (int i = 0; i < n; i++) {
        rt->dist[i] = INF;
        rt->nh[i] = -1;
    }
    rt->dist[s] = 0;
    rt->nh[s] = s;

    for (int cnt = 0; cnt < n; cnt++) {
        int u = -1, best = INF;
        for (int i = 0; i < n; i++) {
            if (!vis[i] && rt->dist[i] < best) {
                best = rt->dist[i];
                u = i;
            }
        }

        if (u == -1 || best == INF) break;
        vis[u] = 1;

        for (int v = 0; v < n; v++) {
            if (!vis[v] && g[u][v] != INF && u != v) {
                int d = rt->dist[u] + g[u][v];
                int step = (u == s) ? v : rt->nh[u];

                if (d < rt->dist[v]) {
                    rt->dist[v] = d;
                    rt->nh[v] = step;
                } else if (d == rt->dist[v] && step < rt->nh[v]) {
                    rt->nh[v] = step;
                }
            }
        }
    }
}

void show_rt(int n, Router rt[], int r) {
    if (r < 0 || r >= n) {
        printf("Invalid Router ID.\n");
        return;
    }
    calc_rt(n, &rt[r]);
    printf("\nRouting Table for Router %d:\n", r);
    printf("%-12s | %-10s | %-10s\n", "Destination", "Next Hop", "Total Cost");
    printf("--------------------------------------\n");
    for (int i = 0; i < n; i++) {
        if (rt[r].dist[i] == INF) {
            printf("%-12d | %-10s | %-10s\n", i, "-", "INF");
        } else {
            printf("%-12d | %-10d | %-10d\n", i, rt[r].nh[i], rt[r].dist[i]);
        }
    }
}

void update_link(int n, int adj[MAXN][MAXN], Router rt[], int seq[]) {
    int u, v, w;
    printf("Enter Router1 Router2 and New Cost (Enter 0 or -1 to remove link): ");
    scanf("%d %d %d", &u, &v, &w);

    if (u < 0 || u >= n || v < 0 || v >= n || u == v) {
        printf("Invalid router IDs.\n");
        return;
    }

    if (w <= 0) {
        adj[u][v] = adj[v][u] = INF;
        printf("\nLink between Router %d and Router %d removed.\n", u, v);
    } else {
        adj[u][v] = adj[v][u] = w;
        printf("\nLink cost between Router %d and Router %d updated to %d.\n", u, v, w);
    }

    sync_nb(n, adj, rt);

    seq[u]++;
    seq[v]++;
    LSA lu = make_lsa(u, n, adj, seq[u]);
    LSA lv = make_lsa(v, n, adj, seq[v]);

    printf("\n--- Updated LSA Generation ---\n");
    show_lsa(lu);
    show_lsa(lv);

    printf("\n--- Flooding Updated LSAs ---\n");
    rt[u].db[u] = lu;
    flood(u, lu, rt);

    rt[v].db[v] = lv;
    flood(v, lv, rt);

    for (int i = 0; i < n; i++) {
        calc_rt(n, &rt[i]);
    }
    printf("\nLSDBs and Routing Tables updated across all routers.\n");
}

int main() {
    int n, m;
    int adj[MAXN][MAXN];
    Router rt[MAXN];
    int seq[MAXN];
    LSA lsas[MAXN];
    int gen_done = 0, hello_done = 0;

    printf("Number of Routers: ");
    scanf("%d", &n);
    printf("Number of Links: ");
    scanf("%d", &m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            adj[i][j] = (i == j) ? 0 : INF;
        }
        seq[i] = 1;
        rt[i].id = i;
        rt[i].deg = 0;
        for (int j = 0; j < MAXN; j++) {
            rt[i].db[j].ok = 0;
        }
    }

    printf("Enter the links (Router1 Router2 Cost):\n");
    for (int i = 0; i < m; i++) {
        int u, v, w;
        scanf("%d %d %d", &u, &v, &w);
        adj[u][v] = adj[v][u] = w;
    }

    int ch, r;
    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. Display Network Topology\n");
        printf("2. Simulate Hello Packet Exchange\n");
        printf("3. Generate LSAs\n");
        printf("4. Simulate LSA Flooding\n");
        printf("5. Display Received LSAs at a Router\n");
        printf("6. Display Routing Table at a Router\n");
        printf("7. Modify / Remove Link\n");
        printf("8. Exit\n");
        printf("Select Option: ");
        scanf("%d", &ch);

        if (ch == 1) {
            show_topo(n, adj);
        } else if (ch == 2) {
            sim_hello(n, adj, rt);
            hello_done = 1;
        } else if (ch == 3) {
            gen_lsas(n, adj, seq, lsas);
            gen_done = 1;
        } else if (ch == 4) {
            if (!hello_done) {
                sync_nb(n, adj, rt);
                hello_done = 1;
            }
            if (!gen_done) {
                gen_lsas(n, adj, seq, lsas);
                gen_done = 1;
            }
            sim_flood(n, rt, lsas);
        } else if (ch == 5) {
            printf("Enter Router ID: ");
            scanf("%d", &r);
            show_db(n, rt, r);
        } else if (ch == 6) {
            printf("Enter Router ID: ");
            scanf("%d", &r);
            show_rt(n, rt, r);
        } else if (ch == 7) {
            update_link(n, adj, rt, seq);
        } else if (ch == 8) {
            break;
        } else {
            printf("Invalid Option.\n");
        }
    }

    return 0;
}