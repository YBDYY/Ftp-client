#include "../include/ftp_client.h"
#include <unistd.h>  
#include <stdio.h>   

void ftp_disconnect(FTPClient *client) {
    if (client == NULL) {
        return;
    }

    // Close the control socket (if it's open)
    if (client->control_socket >= 0) {
        close(client->control_socket);
        client->control_socket = -1;  // Prevent double-close
    }

    // Free the allocated memory for the client
    free(client);
    printf("Disconnected from the FTP server.\n");
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

    printf("Connected to FTP server at %s:%d\n",ip,port);
    return client;
}

int ftp_send_command(FTPClient *client,const char *command){
    if(!client || !command){
        fprintf(stderr,"Error: Invalid client or command\n");
        return -1;
    }


   char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s\r\n", command); // Commands must end with CRLF
    ssize_t sent = send(client->control_socket, buffer, strlen(buffer), 0);
    if (sent < 0) {
        perror("Error: Failed to send command");
        return -1;
    }

    printf("Command sent: %s\n", command);
    return 0;

}

int ftp_read_response(FTPClient *client, char *response, size_t size) {
    if (!client || !response || size == 0) {
        fprintf(stderr, "Error: Invalid arguments to read response\n");
        return -1;
    }

    ssize_t received = recv(client->control_socket, response, size - 1, 0);
    if (received < 0) {
        perror("Error: Failed to read response");
        return -1;
    }

    response[received] = '\0'; // Null-terminate the response
    printf("Response received: %s\n", response);
    return 0;
}