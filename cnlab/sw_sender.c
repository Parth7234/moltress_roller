#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX_DATA 1024
#define TIMEOUT_SEC 2

typedef struct {
    int seq_num;
    char data[MAX_DATA];
} Packet;

typedef struct {
    char ack[4];
    int seq_num;
} AckPacket;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <Receiver IP> <Receiver Port> <Number of Messages>\n", argv[0]);
        exit(1);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int num_messages = atoi(argv[3]);

    int sockfd;
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(port);
    recv_addr.sin_addr.s_addr = inet_addr(ip);

    int current_seq = 0;

    for (int i = 0; i < num_messages; i++) {
        Packet pkt;
        pkt.seq_num = current_seq;
        printf("\nEnter message %d: ", i + 1);
        fgets(pkt.data, MAX_DATA, stdin);
        pkt.data[strcspn(pkt.data, "\n")] = 0; 

        int ack_received = 0;
        while (!ack_received) {
            printf("Transmitting Packet [Seq: %d] | Data: %s\n", pkt.seq_num, pkt.data);
            sendto(sockfd, &pkt, sizeof(pkt), 0, (const struct sockaddr *)&recv_addr, addr_len);

            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);

            struct timeval tv;
            tv.tv_sec = TIMEOUT_SEC;
            tv.tv_usec = 0;

            int activity = select(sockfd + 1, &readfds, NULL, NULL, &tv);

            if (activity == 0) {
                printf("Timeout event! ACK not received. Retransmitting...\n");
            } else if (activity > 0) {
                AckPacket ack_pkt;
                recvfrom(sockfd, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&recv_addr, &addr_len);
                
                if (strcmp(ack_pkt.ack, "ACK") == 0 && ack_pkt.seq_num == current_seq) {
                    printf("ACK received [Seq: %d]\n", ack_pkt.seq_num);
                    ack_received = 1;
                    current_seq = 1 - current_seq; // Toggle sequence number 0/1
                } else {
                    printf("Received unexpected ACK [Seq: %d]. Waiting/Retransmitting...\n", ack_pkt.seq_num);
                }
            } else {
                perror("Select error");
                exit(1);
            }
        }
    }

    close(sockfd);
    return 0;
}