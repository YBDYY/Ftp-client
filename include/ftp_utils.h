#ifndef FTP_UTILS_H
#define FTP_UTILS_H

#include "ftp_client.h"  // Include the FTP client structure for function prototypes

/**
 * Parse an FTP server's response and return a status code.
 * @param response: The server's response string.
 * @return Integer status code based on response.
 */
int parse_ftp_response(const char *response);

/**
 * Validate an FTP server response.
 * @param response: The server's response string.
 * @return true if the response is valid, false otherwise.
 */
bool is_valid_ftp_response(const char *response);

#endif // FTP_UTILS_H
