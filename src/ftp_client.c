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

    response[received] = '\0';
    printf("Response received: %s\n", response);

   
    if (response[0] == '4' || response[0] == '5') {
        fprintf(stderr, "Server error: %s\n", response);
        return -1;  
    }

    return 0;  
}


void ftp_command_loop(FTPClient *client) {
    char command[256];
    char response[512];

    while (1) {
        printf("ftp> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("\nExiting FTP client.\n");
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        if (strncmp(command, "get ", 4) == 0) {
            const char *filename = command + 4;
            if (ftp_get(client, filename) < 0) {
                fprintf(stderr, "Error: Failed to download file '%s'\n", filename);
            }
            continue;
        }

        if (strncmp(command, "put ", 4) == 0) {
            const char *filename = command + 4;
            if (ftp_put(client, filename) < 0) {
                fprintf(stderr, "Error: Failed to upload file '%s'\n", filename);
            }
            continue;
        }

        // For other commands
        if (ftp_send_command(client, command) < 0) {
            fprintf(stderr, "Error sending command.\n");
            continue;
        }

        if (ftp_read_response(client, response, sizeof(response)) < 0) {
            fprintf(stderr, "Error reading response.\n");
            continue;
        }

        if (strncmp(response, "530", 3) == 0) {
            fprintf(stderr, "Server error: %s\n", response);
        }
    }
}


int ftp_send_pasv(FTPClient *client) {
    const char *command = "PASV";
    char response[512];

    
    if (ftp_send_command(client, command) < 0) {
        return -1;
    }

    
    if (ftp_read_response(client, response, sizeof(response)) < 0) {
        return -1;
    }

    
    int a, b, c, d, e, f;
    if (sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
               &a, &b, &c, &d, &e, &f) == 6) {
        
        char ip[16];
        snprintf(ip, sizeof(ip), "%d.%d.%d.%d", a, b, c, d);

        
        int port = e * 256 + f;

        
        printf("PASV response: IP = %s, Port = %d\n", ip, port);

        

        return port;  
    }

    fprintf(stderr, "Failed to parse PASV response: %s\n", response);
    return -1;
}


int ftp_get(FTPClient *client, const char *filename) {
    int port = ftp_send_pasv(client);  
    if (port < 0) {
        return -1;  
    }

   
    int data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket < 0) {
        perror("Error: Failed to create data socket");
        return -1;
    }

    struct sockaddr_in data_addr;
    memset(&data_addr, 0, sizeof(data_addr));
    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(port);
    data_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  

    
    if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) {
        perror("Error: Failed to connect to data port");
        close(data_socket);
        return -1;
    }

    
    char command[512];
    snprintf(command, sizeof(command), "RETR %s", filename);
    if (ftp_send_command(client, command) < 0) {
        close(data_socket);
        return -1;
    }

    
    char response[512];
    if (ftp_read_response(client, response, sizeof(response)) < 0) {
        close(data_socket);
        return -1;
    }

    
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("ERROR: Failed to open file for writing");
        close(data_socket);
        return -1;
    }

    
    char buffer[1024];
    ssize_t bytes_received;
    while ((bytes_received = recv(data_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    close(data_socket);

   
    if (ftp_read_response(client, response, sizeof(response)) < 0) {
        return -1;
    }

    printf("File %s downloaded successfully.\n", filename);
    return 0;
}



int ftp_put(FTPClient *client, const char *filename) {
    
    int port = ftp_send_pasv(client);
    if (port < 0) {
        return -1;  
    }

    
    int data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket < 0) {
        perror("Error: Failed to create data socket");
        return -1;
    }

    struct sockaddr_in data_addr;
    memset(&data_addr, 0, sizeof(data_addr));
    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(port);
    data_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  

    if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) {
        perror("Error: Failed to connect to data port");
        close(data_socket);
        return -1;
    }

   
    char command[512];
    snprintf(command, sizeof(command), "STOR %s", filename);
    if (ftp_send_command(client, command) < 0) {
        close(data_socket);
        return -1;
    }

    
    char response[512];
    if (ftp_read_response(client, response, sizeof(response)) < 0) {
        close(data_socket);
        return -1;
    }

   
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error: Failed to open file for reading");
        close(data_socket);
        return -1;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(data_socket, buffer, bytes_read, 0);
    }

    fclose(file);
    close(data_socket);

    
    if (ftp_read_response(client, response, sizeof(response)) < 0) {
        return -1;
    }

    printf("File %s uploaded successfully.\n", filename);
    return 0;
}
