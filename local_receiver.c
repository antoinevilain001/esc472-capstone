// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[1024];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Zero out servaddr and cliaddr
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Bind to localhost:PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept from any address
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Receiver is ready on port %d\n", PORT);

    // Wait for incoming packet
    len = sizeof(cliaddr);
    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);

        // Send reply back
        sendto(sockfd, "ACK", strlen("ACK"), 0, (struct sockaddr *)&cliaddr, len);
        printf("Sent ACK\n");
    }

    close(sockfd);
    return 0;
}
