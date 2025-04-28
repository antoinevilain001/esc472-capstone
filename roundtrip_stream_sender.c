// roundtrip_stream_sender.c (with RTT measurement during streaming)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define PORT 5000
#define SERVER_IP "192.168.10.3"
#define PACKET_COUNT 100

static inline long long current_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (long long)(ts.tv_sec) * 1000000LL + (ts.tv_nsec) / 1000;
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, recvaddr;
    int small_buf_size = 256;
    long long send_times[PACKET_COUNT] = {0};  // record when each packet is sent
    int send_index = 0, recv_index = 0;
    char buffer[1024];
    socklen_t len = sizeof(recvaddr);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set small buffers
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &small_buf_size, sizeof(small_buf_size));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &small_buf_size, sizeof(small_buf_size));

    // Setup destination address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

    printf("Streaming %d packets to %s:%d...\n", PACKET_COUNT, SERVER_IP, PORT);

    // Set socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    long long start_time = current_time_us();
    long long next_send_time = start_time;
    int sent_all = 0;
    int received = 0;

    while (received < PACKET_COUNT) {
        long long now = current_time_us();

        // Check if time to send next packet
        if (!sent_all && now >= next_send_time) {
            if (send_index < PACKET_COUNT) {
                sendto(sockfd, "StreamingData", strlen("StreamingData"), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
                send_times[send_index] = now;
                printf("Sent packet %d at %lld us\n", send_index + 1, now);
                send_index++;
                next_send_time += 1000; // 1 ms later
            } else {
                sent_all = 1;
            }
        }

        // Always check for incoming replies
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&recvaddr, &len);
        if (n > 0) {
            long long ack_time = current_time_us();
            if (recv_index < PACKET_COUNT) {
                long long rtt = ack_time - send_times[recv_index];
                printf("ACK for packet %d: RTT = %lld us\n", recv_index + 1, rtt);
                recv_index++;
            }
        }
        // Sleep a little to avoid burning CPU
        usleep(100);
    }

    printf("All packets sent and ACKed!\n");
    close(sockfd);
    return 0;
}
