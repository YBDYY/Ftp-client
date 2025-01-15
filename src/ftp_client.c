#include "../include/ftp_client.h"
#include <unistd.h>  // For close()
#include <stdio.h>   // For printf()

void ftp_disconnect(FTPClient *client) {
    if (client == NULL) {
        return;
    }

   
    close(client->control_socket);
    printf("Disconnected from the FTP server.\n");
    
   
    free(client);
}

FTPClient* ftp_connect(const char *ip, int port) {
    FTPClient *client = malloc(sizeof(FTPClient));
    if (!client) {
        perror("Error: Failed to allocate memory for FTPClient");
        return NULL;
    }

    client->control_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->control_socket < 0) {
        perror("Error: Failed to create socket");
        free(client);
        return NULL;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // Clear memory
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_port = htons(port);           // Convert port to network byte order

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Error: Invalid IP address or not supported");
        close(client->control_socket);
        free(client);
        return NULL;
    }

    if (connect(client->control_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error: Failed to connect to the server");
        close(client->control_socket);
        free(client);
        return NULL;
    }

    return client;
}
