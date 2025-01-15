#include "../include/ftp_client.h"

int main() {
    // Connect to the FTP server
    FTPClient *client = ftp_connect("127.0.0.1", 21);

    if (client) {
        printf("Connected to the FTP server at 127.0.0.1:21\n");

        // Perform any necessary FTP actions here

        // Call ftp_disconnect to clean up and free resources
        ftp_disconnect(client);
    } else {
        printf("Failed to connect to the FTP server.\n");
    }

    return 0;
}
