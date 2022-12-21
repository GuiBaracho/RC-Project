#ifndef _UDP_GS_
#define _UDP_GS_

void udp_server(std::string word_file, std::string GSPort, int v_mode);
void error_message(std::string command, std::string PLID, int v_mode, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void server_start(std::string word_file, std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
int value(std::string command);
int createFileONGame(std::string PLID, std::string &wordhint);
void searchOnFile(std::string PLID, std::string trial, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void server_play(std::string PLID, std::string trial, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void server_guess();
void server_quit(std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
std::string getTimeInString(std::string state);

#endif