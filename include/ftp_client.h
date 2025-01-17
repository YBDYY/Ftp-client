#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h> // For `bool`, `true`, `false`

#define BUFFER_SIZE 1024 // Size of buffer for commands and responses

/**
 * Struct representing an FTP client.
 */
typedef struct {
    int control_socket;                 // Socket for the control connection
    char buffer[BUFFER_SIZE];           // Buffer for temporary data
    char server_ip[INET_ADDRSTRLEN];    // Server IP address as a string
    int server_port;                    // Server port number
} FTPClient;

/**
 * Connect to an FTP server.
 * @param ip: The server's IP address.
 * @param port: The server's port number.
 * @return Pointer to an FTPClient instance if successful, NULL otherwise.
 */
FTPClient* ftp_connect(const char *ip, int port);

/**
 * Disconnect from the FTP server.
 * @param client: Pointer to the FTPClient instance.
 */
void ftp_disconnect(FTPClient *client);

/**
 * Send a command to the FTP server.
 * @param client: Pointer to the FTPClient instance.
 * @param command: The FTP command to send.
 * @return true if the command was sent successfully, false otherwise.
 */
int ftp_send_command(FTPClient *client, const char *command);

/**
 * Read the server's response.
 * @param client: Pointer to the FTPClient instance.
 * @param response: Buffer to store the server's response.
 * @param response_size: Size of the response buffer.
 * @return true if the response was read successfully, false otherwise.
 */
int ftp_read_response(FTPClient *client, char *response, size_t response_size);


void ftp_command_loop(FTPClient *client);

#endif // FTP_CLIENT_H
