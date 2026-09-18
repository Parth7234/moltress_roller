#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 2048

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    // Requested receive-buffer size[cite: 1]
    int rcvbuf_size = 8192; // 8 KB

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Set a small receive-buffer size using setsockopt()[cite: 1]
    setsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, sizeof(rcvbuf_size));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);
    
    printf("Server listening... (Buffer size: %d bytes)\n", rcvbuf_size);
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    long total_bytes = 0;
    int valread;
    
    // Receive data in fixed-size blocks[cite: 1]
    while ((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
        total_bytes += valread;
        // Display the cumulative number of bytes received[cite: 1]
        printf("Cumulative bytes received: %ld\n", total_bytes);
        
        // Introduce a delay after each recv() to simulate a slow application[cite: 1]
        usleep(100000); // 100 ms
    }
    
    close(new_socket);
    close(server_fd);
    return 0;
}