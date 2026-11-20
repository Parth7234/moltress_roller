#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define MAX_ROUTES 100
#define LOCAL_AS 100

typedef struct {
    char prefix_str[32];
    uint32_t network_ip;
    int prefix_len;
    char next_hop_str[32];
    uint32_t next_hop_ip;
    char as_path[256];
    int as_path_len;
    int local_pref;
    bool active;
} Route;

Route routing_table[MAX_ROUTES];
int route_count = 0;

// Helper: Parse prefix "192.168.1.0/24" into IP and length
void parse_prefix(const char *prefix_str, uint32_t *ip, int *len) {
    char temp[32];
    strcpy(temp, prefix_str);
    char *token = strtok(temp, "/");
    struct in_addr addr;
    inet_pton(AF_INET, token, &addr);
    *ip = ntohl(addr.s_addr);
    
    token = strtok(NULL, "/");
    if (token) {
        *len = atoi(token);
    } else {
        *len = 32;
    }
}

// Helper: Check for AS_PATH loop and calculate path length
bool check_as_loop_and_length(const char *as_path, int *length) {
    char temp[256];
    strcpy(temp, as_path);
    char *token = strtok(temp, " ");
    *length = 0;
    
    while (token != NULL) {
        if (atoi(token) == LOCAL_AS) {
            return true; // Loop detected
        }
        (*length)++;
        token = strtok(NULL, " ");
    }
    return false;
}

// Compare two routes for BGP Best-Path Selection. Returns true if r1 is better than r2.
bool is_better_route(Route r1, Route r2) {
    if (r1.local_pref != r2.local_pref) {
        printf("higher local pref\n");
        return r1.local_pref > r2.local_pref;
    }
    if (r1.as_path_len != r2.as_path_len) {
        printf("less path lenght\n");
        return r1.as_path_len < r2.as_path_len;
    }
    printf("lower ip\n");
    return r1.next_hop_ip < r2.next_hop_ip;
}

void add_route() {
    if (route_count >= MAX_ROUTES) {
        printf("Routing table is full!\n");
        return;
    }

    Route new_route;
    printf("Destination Prefix (e.g., 10.0.0.0/8): ");
    scanf("%s", new_route.prefix_str);
    parse_prefix(new_route.prefix_str, &new_route.network_ip, &new_route.prefix_len);

    printf("Next Hop: ");
    scanf("%s", new_route.next_hop_str);
    struct in_addr nh_addr;
    inet_pton(AF_INET, new_route.next_hop_str, &nh_addr);
    new_route.next_hop_ip = ntohl(nh_addr.s_addr);

    printf("AS_PATH: ");
    getchar(); // clear newline
    fgets(new_route.as_path, sizeof(new_route.as_path), stdin);
    new_route.as_path[strcspn(new_route.as_path, "\n")] = 0; // remove newline

    if (check_as_loop_and_length(new_route.as_path, &new_route.as_path_len)) {
        printf("Route rejected: AS_PATH contains local AS%d.\n", LOCAL_AS);
        return;
    }

    printf("LOCAL_PREF: ");
    scanf("%d", &new_route.local_pref);
    new_route.active = true;

    routing_table[route_count++] = new_route;
    printf("Route added successfully.\n");
}

void display_all_routes() {
    printf("\n--- All Stored BGP Routes ---\n");
    for (int i = 0; i < route_count; i++) {
        if (routing_table[i].active) {
            printf("Prefix: %s | Next Hop: %s | AS_PATH: %s | LOCAL_PREF: %d\n",
                   routing_table[i].prefix_str, routing_table[i].next_hop_str,
                   routing_table[i].as_path, routing_table[i].local_pref);
        }
    }
}

void display_best_routes() {
    printf("\n--- Best Routes ---\n");
    bool processed[MAX_ROUTES] = {false};
    int count = 1;

    for (int i = 0; i < route_count; i++) {
        if (!routing_table[i].active || processed[i]) continue;

        Route best = routing_table[i];
        bool flag=false;
        for (int j = i + 1; j < route_count; j++) {
            if (routing_table[j].active && strcmp(routing_table[i].prefix_str, routing_table[j].prefix_str) == 0) {
                processed[j] = true;
                if (is_better_route(routing_table[j], best)) {
                    best = routing_table[j];
                    flag=true;
                }
            }
        }
        if(!flag)printf("Only available route\n");
        printf("%d. Prefix: %s\n", count++, best.prefix_str);
        printf("   Best Next Hop: %s\n", best.next_hop_str);
        printf("   LOCAL_PREF: %d, AS_PATH length: %d\n", best.local_pref, best.as_path_len);
    }
}

void forward_packet() {
    char dest_str[32];
    printf("Destination IP: ");
    scanf("%s", dest_str);

    struct in_addr dest_addr;
    inet_pton(AF_INET, dest_str, &dest_addr);
    uint32_t dest_ip = ntohl(dest_addr.s_addr);

    Route *best_match = NULL;
    int longest_len = -1;

    for (int i = 0; i < route_count; i++) {
        if (!routing_table[i].active) continue;

        uint32_t mask = routing_table[i].prefix_len == 0 ? 0 : (~0U << (32 - routing_table[i].prefix_len));
        if ((dest_ip & mask) == (routing_table[i].network_ip & mask)) {
            // Longest prefix match
            if (routing_table[i].prefix_len > longest_len) {
                longest_len = routing_table[i].prefix_len;
                best_match = &routing_table[i];
            } 
            // BGP Best-Path tie-breaker for same prefix length
            else if (routing_table[i].prefix_len == longest_len && strcmp(routing_table[i].prefix_str, best_match->prefix_str) == 0) {
                if (is_better_route(routing_table[i], *best_match)) {
                    best_match = &routing_table[i];
                }
            }
        }
    }

    if (best_match) {
        printf("Expected Matched Prefix: %s\n", best_match->prefix_str);
        printf("Expected Next Hop: %s\n", best_match->next_hop_str);
        printf("LOCAL_PREF: %d\n", best_match->local_pref);
        printf("AS_PATH: %s\n", best_match->as_path);
    } else {
        printf("Expected Output: No route found.\n");
    }
}

void withdraw_route() {
    char prefix[32], next_hop[32];
    printf("Destination Prefix: ");
    scanf("%s", prefix);
    printf("Next Hop: ");
    scanf("%s", next_hop);

    bool found = false;
    for (int i = 0; i < route_count; i++) {
        if (routing_table[i].active && 
            strcmp(routing_table[i].prefix_str, prefix) == 0 && 
            strcmp(routing_table[i].next_hop_str, next_hop) == 0) {
            routing_table[i].active = false;
            found = true;
            printf("Expected Output: Route withdrawn successfully.\n");
            break;
        }
    }
    if (!found) {
        printf("Route not found.\n");
    }
}

int main() {
    int choice;
    while (1) {
        printf("\n--- BGP Router Menu (AS%d) ---\n", LOCAL_AS);
        printf("1. Add BGP Route\n");
        printf("2. Display All Routes\n");
        printf("3. Display Best Routes\n");
        printf("4. Forward Packet\n");
        printf("5. Withdraw Route\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_route(); break;
            case 2: display_all_routes(); break;
            case 3: display_best_routes(); break;
            case 4: forward_packet(); break;
            case 5: withdraw_route(); break;
            case 6: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}