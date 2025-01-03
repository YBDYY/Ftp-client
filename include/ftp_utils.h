#ifndef FTP_UTILS_H
#define FTP_UTILS_H

void handle_error(const char *message);
char *parse_response(const char *response);

#endif 