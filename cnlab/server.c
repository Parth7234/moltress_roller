#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Structure to hold the statistics
struct Stats {
    int min;
    int max;
    int sum;
    float average;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int server_port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    // Bind socket to port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen for connections
    listen(server_socket, 5);
    printf("Server listening on port %d...\n", server_port);

    while (1) {
        // Accept a connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        // Display client IP and port
        printf("\nConnection accepted from Client IP: %s, Port: %d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int n;
        // Receive number of integers (N)
        if (recv(client_socket, &n, sizeof(int), 0) <= 0) {
            printf("Client disconnected or error occurred.\n");
            close(client_socket);
            continue;
        }

        // Receive the array of integers
        int *values = (int *)malloc(n * sizeof(int));
        recv(client_socket, values, n * sizeof(int), 0);

        // Compute statistics
        struct Stats results;
        results.min = values[0];
        results.max = values[0];
        results.sum = 0;

        printf("Integers received: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", values[i]);
            if (values[i] < results.min) results.min = values[i];
            if (values[i] > results.max) results.max = values[i];
            results.sum += values[i];
        }
        printf("\n");
        results.average = (float)results.sum / n;

        // Display computed results on server
        printf("Computed - Min: %d, Max: %d, Sum: %d, Average: %.2f\n", 
               results.min, results.max, results.sum, results.average);

        // Send results back to client
        send(client_socket, &results, sizeof(struct Stats), 0);

        free(values);
        close(client_socket);
        printf("Client disconnected.\n");
    }

    close(server_socket);
    return 0;
}