#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef struct {
    int control_socket;
    char buffer[BUFFER_SIZE];
    char server_ip[INET_ADDRSTRLEN];
    int server_port;
}FTPClient;


FTPClient* ftp_connect(const char *ip, int port);  
void ftp_disconnect(FTPClient *client);           
bool ftp_send_command(FTPClient *client, const char *command); 
bool ftp_read_response(FTPClient *client, char *response, size_t response_size); 

#endif 