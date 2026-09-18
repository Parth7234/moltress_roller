#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_DATA 1024

typedef struct {
    int seq_num;
    char data[MAX_DATA];
} Packet;

typedef struct {
    char ack[4];
    int seq_num;
} AckPacket;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in recv_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = INADDR_ANY;
    recv_addr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("Receiver listening on port %d...\n", port);

    int expected_seq = 0;
    int packet_counter = 0;

    while (1) {
        Packet pkt;
        recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client_addr, &addr_len);
        packet_counter++;

        printf("\nReceived Packet [Seq: %d]\n", pkt.seq_num);

        if (pkt.seq_num == expected_seq) {
            printf("Expected sequence received. Delivering data: %s\n", pkt.data);
            expected_seq = 1 - expected_seq; // Toggle expected sequence
        } else {
            printf("Duplicate packet detected! Avoiding double delivery.\n");
        }

        // Simulate ACK Loss: Drop every 5th packet
        if (packet_counter % 5 == 0) {
            printf("Simulating loss: Deliberately dropping ACK for Seq %d\n", pkt.seq_num);
            continue; 
        }

        AckPacket ack_pkt;
        strcpy(ack_pkt.ack, "ACK");
        ack_pkt.seq_num = pkt.seq_num; 

        printf("Sending ACK [Seq: %d]\n", ack_pkt.seq_num);
        sendto(sockfd, &ack_pkt, sizeof(ack_pkt), 0, (const struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}