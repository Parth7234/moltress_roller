#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT 8080
#define CHUNK_SIZE 4096
#define TOTAL_PAYLOAD 5000000 // Send ~5MB of data

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Server IP>\n", argv[0]);
        return 1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[CHUNK_SIZE];
    memset(buffer, 'A', CHUNK_SIZE); 

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    struct timeval start, end;
    gettimeofday(&start, NULL);

    long bytes_sent = 0;
    
    // Send a large amount of data continuously[cite: 1]
    while (bytes_sent < TOTAL_PAYLOAD) {
        int sent = send(sock, buffer, CHUNK_SIZE, 0);
        if (sent < 0) break;
        bytes_sent += sent;
    }

    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    // Display total number of bytes sent and total transfer time[cite: 1]
    printf("Transfer Complete.\n");
    printf("Total bytes sent: %ld\n", bytes_sent);
    printf("Total transfer time: %.3f seconds\n", time_taken);

    close(sock);
    return 0;
}