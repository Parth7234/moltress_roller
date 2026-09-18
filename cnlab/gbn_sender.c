#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define MAX_PKTS 50
#define PACKET_SIZE 1024
#define TIMEOUT_SEC 2

struct packet {
    int seq_num;
    char data[PACKET_SIZE];
};

struct ack_packet {
    int ack_num;
};

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <IP> <Port> <Total Packets> <Window Size>\n", argv[0]);
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int total_packets = atoi(argv[3]);
    int window_size = atoi(argv[4]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int base = 0;
    int next_seq = 0;
    int retransmissions = 0;
    struct packet pkt;
    struct ack_packet ack;

    while (base < total_packets) {
        // Send packets up to window size
        while (next_seq < base + window_size && next_seq < total_packets) {
            pkt.seq_num = next_seq;
            sprintf(pkt.data, "Data Payload for Packet %d", next_seq);
            sendto(sockfd, &pkt, sizeof(pkt), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
            printf("Transmitted Packet: %d (Window: %d to %d)\n", next_seq, base, base + window_size - 1);
            next_seq++;
        }

        // Wait for cumulative ACK
        socklen_t len = sizeof(servaddr);
        if (recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *) &servaddr, &len) < 0) {
            printf("Timeout event! Retransmitting window starting from %d\n", base);
            next_seq = base; // Go-Back-N behavior
            retransmissions++;
        } else {
            printf("Received ACK: %d\n", ack.ack_num);
            if (ack.ack_num >= base) {
                base = ack.ack_num + 1; // Slide window
            }
        }
    }

    printf("Transfer complete. Total retransmissions: %d\n", retransmissions);
    close(sockfd);
    return 0;
}