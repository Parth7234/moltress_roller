#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define TCP_PORT 8080
#define UDP_PORT 8081
#define MAX_RECORDS 5
#define BUFFER_SIZE 256

// Predefined DNS-like records
typedef struct {
    char hostname[64];
    char type[10];
    char value[64];
} Record;

Record dns_table[MAX_RECORDS] = {
    {"www.example.com", "A", "192.168.1.10"},
    {"mail.example.com", "A", "192.168.1.20"}, // Address records
    {"web.example.com", "CNAME", "www.example.com"}, // Canonical name
    {"example.com", "MX", "mail.example.com"}, // Mail records
    {"example.com", "NS", "ns1.example.com"} // Name server records
};

int active_clients = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Problem 1: TCP thread per client
void* handle_tcp_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0) break; // Client disconnected
        
        char host[64], type[10];
        if (sscanf(buffer, "%s %s", host, type) == 2) {
            char response[BUFFER_SIZE] = "Record not found\n";
            for (int i = 0; i < MAX_RECORDS; i++) {
                if (strcmp(dns_table[i].hostname, host) == 0 && strcmp(dns_table[i].type, type) == 0) {
                    snprintf(response, BUFFER_SIZE, "%s\n", dns_table[i].value);
                    break;
                }
            }
            send(client_socket, response, strlen(response), 0);
        }
    }
    
    close(client_socket);
    pthread_mutex_lock(&lock);
    active_clients--;
    pthread_mutex_unlock(&lock);
    return NULL;
}

// Problem 2: UDP Server Status
void* handle_udp_status(void* arg) {
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_PORT);
    bind(udp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(udp_socket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &addr_len);
        
        if (strncmp(buffer, "STATUS", 6) == 0) {
            pthread_mutex_lock(&lock);
            int current_clients = active_clients;
            pthread_mutex_unlock(&lock);
            
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "Server active. Connected clients: %d\n", current_clients);
            sendto(udp_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, addr_len);
        }
    }
    return NULL;
}

int main() {
    // Start UDP service thread
    pthread_t udp_thread;
    pthread_create(&udp_thread, NULL, handle_udp_status, NULL);
    
    // Setup TCP service
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(TCP_PORT);
    
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 10);
    printf("TCP Server running on port %d, UDP on port %d...\n", TCP_PORT, UDP_PORT);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int* client_socket = malloc(sizeof(int));
        *client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        
        // Display IP and Port as required
        printf("Client connected: IP %s, Port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        pthread_mutex_lock(&lock);
        active_clients++;
        pthread_mutex_unlock(&lock);
        
        pthread_t tcp_thread;
        pthread_create(&tcp_thread, NULL, handle_tcp_client, client_socket);
        pthread_detach(tcp_thread);
    }
    return 0;
}