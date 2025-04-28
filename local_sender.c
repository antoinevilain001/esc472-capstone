// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 5000
#define SERVER_IP "127.0.0.1" // localhost for now

// Function to get microsecond-precision timestamp
static inline long long current_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)(ts.tv_sec) * 1000000LL + (ts.tv_nsec) / 1000;
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];
    socklen_t len;
    long long start, end;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

    // Start timing
    start = current_time_us();

    // Send message
    sendto(sockfd, "Hello", strlen("Hello"), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Message sent\n");

    // Wait for reply
    len = sizeof(servaddr);
    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&servaddr, &len);
    if (n < 0) {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';

    // Stop timing
    end = current_time_us();

    printf("Received: %s\n", buffer);
    printf("Round-trip time: %lld microseconds\n", end - start);

    close(sockfd);
    return 0;
}
