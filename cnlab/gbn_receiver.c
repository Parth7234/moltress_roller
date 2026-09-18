#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

    int expected_seq = 0;
    int pkt_count = 0;
    struct packet pkt;
    struct ack_packet ack;
    socklen_t len = sizeof(cliaddr);

    printf("Receiver listening on port %d...\n", port);

    while (1) {
        recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *) &cliaddr, &len);
        pkt_count++;

        // Simulate packet loss using a predetermined pattern
        if (pkt_count % LOSS_PROBABILITY == 0) {
            printf("Simulated loss for Packet %d\n", pkt.seq_num);
            continue; 
        }

        if (pkt.seq_num == expected_seq) {
            printf("Successfully received in-order packet: %d\n", pkt.seq_num);
            ack.ack_num = expected_seq;
            expected_seq++;
        } else {
            printf("Out-of-order packet: %d. Expected: %d. Sending duplicate ACK.\n", pkt.seq_num, expected_seq);
            ack.ack_num = expected_seq - 1; // Re-ack the last successfully received packet
        }

        sendto(sockfd, &ack, sizeof(ack), 0, (const struct sockaddr *) &cliaddr, len);
    }

    close(sockfd);
    return 0;
}