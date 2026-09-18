 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define MAX_PKTS 100
#define PACKET_SIZE 1024
#define TIMEOUT_MS 2000 // 2 seconds

struct packet {
    int seq_num;
    char data[PACKET_SIZE];
};

struct ack_packet {
    int ack_num;
};

// Helper function to get current time in milliseconds
long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

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

    int base = 0;
    int next_seq = 0;
    int retransmissions = 0;
    
    int acked[MAX_PKTS] = {0};
    long send_time[MAX_PKTS] = {0};
    struct packet pkt;
    struct ack_packet ack;

    fd_set readfds;
    struct timeval timeout;

    while (base < total_packets) {
        // Send new packets within the window
        while (next_seq < base + window_size && next_seq < total_packets) {
            pkt.seq_num = next_seq;
            sprintf(pkt.data, "Data Payload for Packet %d", next_seq);
            sendto(sockfd, &pkt, sizeof(pkt), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
            send_time[next_seq] = get_time_ms();
            printf("Transmitted Packet: %d\n", next_seq);
            next_seq++;
        }

        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000; // 10ms short poll

        int activity = select(sockfd + 1, &readfds, NULL, NULL, &timeout);

        if (activity > 0 && FD_ISSET(sockfd, &readfds)) {
            socklen_t len = sizeof(servaddr);
            recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *) &servaddr, &len);
            printf("Received ACK: %d\n", ack.ack_num);
            
            if (ack.ack_num >= base && ack.ack_num < total_packets) {
                acked[ack.ack_num] = 1;
            }

            // Slide the window if the base packet is acknowledged
            while (base < total_packets && acked[base]) {
                base++;
                printf("Window shifted. New base: %d\n", base);
            }
        }

        // Check for individual packet timeouts[cite: 1]
        long current_time = get_time_ms();
        for (int i = base; i < next_seq; i++) {
            if (!acked[i] && (current_time - send_time[i] > TIMEOUT_MS)) {
                pkt.seq_num = i;
                sprintf(pkt.data, "Data Payload for Packet %d", i);
                sendto(sockfd, &pkt, sizeof(pkt), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
                send_time[i] = current_time; // Reset timer
                retransmissions++;
                printf("Timeout! Retransmitted individual Packet: %d\n", i);
            }
        }
    }

    printf("Selective Repeat Transfer complete. Total retransmissions: %d\n", retransmissions);
    close(sockfd);
    return 0;
}