#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct Stats {
    int min;
    int max;
    int sum;
    float average;
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);
    int client_socket;
    struct sockaddr_in server_addr;

    // Create TCP socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Connected to server.\n");

    // Read inputs from user
    int n;
    printf("N = ");
    scanf("%d", &n);

    int *values = (int *)malloc(n * sizeof(int));
    printf("Values: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &values[i]);
    }

    // Send N and the values to the server
    send(client_socket, &n, sizeof(int), 0);
    send(client_socket, values, n * sizeof(int), 0);

    // Receive the computed statistics
    struct Stats results;
    if (recv(client_socket, &results, sizeof(struct Stats), 0) > 0) {
        // Display results
        printf("Minimum = %d\n", results.min);
        printf("Maximum = %d\n", results.max);
        printf("Sum = %d\n", results.sum);
        printf("Average = %.2f\n", results.average);
    } else {
        printf("Failed to receive data from server.\n");
    }

    free(values);
    close(client_socket);
    return 0;
}