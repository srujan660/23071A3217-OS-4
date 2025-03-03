#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to Shopping Server\n");

    while (1) {
        printf("\n1) View Products\n2) Add to Cart\n3) Exit\nEnter option: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock_fd, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);
        read(sock_fd, buffer, BUFFER_SIZE);
        printf("%s", buffer);

        if (strncmp(buffer, "Goodbye", 7) == 0) {
            close(sock_fd);
            break;
        }
    }

    return 0;
}
