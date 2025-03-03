#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    int id;
    char name[50];
    float price;
    int stock;
} Product;

Product inventory[] = {
    {1, "Laptop", 700.00, 10},
    {2, "Phone", 400.00, 15},
    {3, "Headphones", 50.00, 20},
    {4, "Mouse", 20.00, 25}
};

int inventory_size = sizeof(inventory) / sizeof(Product);

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int option, product_id, quantity;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        read(client_socket, buffer, BUFFER_SIZE);
        option = atoi(buffer);

        if (option == 1) {  // List products
            char response[BUFFER_SIZE] = "Available Products:\n";
            for (int i = 0; i < inventory_size; i++) {
                char item[100];
                sprintf(item, "%d) %s - %.2f [%d in stock]\n", 
                        inventory[i].id, inventory[i].name, inventory[i].price, inventory[i].stock);
                strcat(response, item);
            }
            send(client_socket, response, strlen(response), 0);
        } else if (option == 2) {  // Add to cart
            send(client_socket, "Enter product ID: ", 18, 0);
            memset(buffer, 0, BUFFER_SIZE);
            read(client_socket, buffer, BUFFER_SIZE);
            product_id = atoi(buffer);

            send(client_socket, "Enter quantity: ", 17, 0);
            memset(buffer, 0, BUFFER_SIZE);
            read(client_socket, buffer, BUFFER_SIZE);
            quantity = atoi(buffer);

            if (product_id > 0 && product_id <= inventory_size && inventory[product_id - 1].stock >= quantity) {
                inventory[product_id - 1].stock -= quantity;
                char response[100];
                sprintf(response, "Added %d %s(s) to cart.\n", quantity, inventory[product_id - 1].name);
                send(client_socket, response, strlen(response), 0);
            } else {
                send(client_socket, "Invalid product or insufficient stock.\n", 38, 0);
            }
        } else if (option == 3) {  // Exit
            send(client_socket, "Goodbye!\n", 9, 0);
            close(client_socket);
            break;
        }
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd == -1) {
            perror("Client accept failed");
            continue;
        }
        printf("New client connected.\n");
        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}
