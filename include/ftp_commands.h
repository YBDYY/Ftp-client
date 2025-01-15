#ifndef FTP_COMMANDS_H
#define FTP_COMMANDS_H

#include "ftp_client.h"  // Include the FTP client structure for function prototypes

/**
 * Send the USER command to the FTP server.
 * @param client: Pointer to the FTPClient instance.
 * @param username: The username to authenticate with.
 * @return true if the command was sent successfully, false otherwise.
 */
bool send_user_command(FTPClient *client, const char *username);

/**
 * Send the PASS command to the FTP server.
 * @param client: Pointer to the FTPClient instance.
 * @param password: The password to authenticate with.
 * @return true if the command was sent successfully, false otherwise.
 */
bool send_pass_command(FTPClient *client, const char *password);

/**
 * Send the QUIT command to the FTP server.
 * @param client: Pointer to the FTPClient instance.
 * @return true if the command was sent successfully, false otherwise.
 */
bool send_quit_command(FTPClient *client);

#endif // FTP_COMMANDS_H
