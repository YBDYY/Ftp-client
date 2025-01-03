#ifndef FTP_COMMANDS_H
#define FTP_COMMANDS_H

void send_command(int sock, const char *command);
void list_files(int sock);
void download_file(int sock, const char *filename);

#endif 