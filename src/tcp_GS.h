#ifndef _TCP_GS_
#define _TCP_GS_

int FindLastGame(const char* PLID, char* fname);
void server_hint(int fd, std::string PLID, std::string &send);
void handleTCP(int fd);
void tcp_server(std::string word_file, std::string GSPort);

#endif