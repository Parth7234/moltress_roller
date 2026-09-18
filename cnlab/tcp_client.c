#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 256

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    char host[64], type[10], buffer[BUFFER_SIZE];
    
    while (1) {
        printf("Enter domain (or EXIT to quit): ");
        scanf("%s", host);
        if (strcmp(host, "EXIT") == 0) break;
        
        printf("Enter type: ");
        scanf("%s", type);
        
        snprintf(buffer, BUFFER_SIZE, "%s %s", host, type);
        send(sock, buffer, strlen(buffer), 0);
        
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE - 1, 0);
        printf("Server response:\n%s\n", buffer);
    }
    
    close(sock);
    return 0;
}