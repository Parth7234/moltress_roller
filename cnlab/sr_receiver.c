#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_PKTS 100
#define PACKET_SIZE 1024
#define LOSS_PROBABILITY 5 // Drop 1 in every 5 packets

struct packet {
    int seq_num;
    char data[PACKET_SIZE];
};

struct ack_packet {
    int ack_num;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in servaddr, cliaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    
    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    int base = 0;
    int window_size = 4; // Expected window size N
    int buffered[MAX_PKTS] = {0}; // Buffer for out-of-order packets[cite: 1]
    int pkt_count = 0;
    
    struct packet pkt;
    struct ack_packet ack;
    socklen_t len = sizeof(cliaddr);

    printf("Selective Repeat Receiver listening on port %d...\n", port);

    while (1) {
        recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *) &cliaddr, &len);
        pkt_count++;

        // Simulate packet loss using a predetermined packet-loss pattern[cite: 1]
        if (pkt_count % LOSS_PROBABILITY == 0) {
            printf("Simulated loss for Packet %d\n", pkt.seq_num);
            continue; 
        }

        // Send individual acknowledgments[cite: 1]
        ack.ack_num = pkt.seq_num;
        sendto(sockfd, &ack, sizeof(ack), 0, (const struct sockaddr *) &cliaddr, len);
        printf("Sent individual ACK for: %d\n", ack.ack_num);

        if (pkt.seq_num >= base && pkt.seq_num < base + window_size) {
            if (!buffered[pkt.seq_num]) {
                buffered[pkt.seq_num] = 1; // Accept correctly received out-of-order packets[cite: 1]
                printf("Buffered Packet: %d\n", pkt.seq_num);
            } else {
                printf("Duplicate Packet detected and ignored: %d\n", pkt.seq_num);
            }
        } else if (pkt.seq_num < base) {
             printf("Received packet before window (already delivered): %d\n", pkt.seq_num);
        }

        // Deliver buffered packets in order and advance the receive window
        while (buffered[base]) {
            printf("Delivering Packet to Application: %d\n", base);
            base++;
            printf("Receiver window shifted. Expected Base is now: %d\n", base);
        }
    }

    close(sockfd);
    return 0;
}