#ifndef _UDP_GS_
#define _UDP_GS_

void udp_server(std::string word_file, std::string GSPort, int v_mode);
void error_message(std::string command, std::string PLID, int v_mode, int &fd, struct addrinfo &hints, struct addrinfo *&res);
void server_start(std::string word_file, std::string PLID, int &fd, struct addrinfo &hints, struct addrinfo *&res);
int value(std::string command);
int searchONGame(std::string PLID, std::string gword);

#endif