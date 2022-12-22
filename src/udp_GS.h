#ifndef _UDP_GS_
#define _UDP_GS_

int value(std::string command);
int calculate_maxerrors(std::string word);
std::string toStringT(int dt);
std::string getTimeInString(std::string state);
void error_message(std::string command, std::string PLID, int v_mode, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
int createFile(std::string PLID, std::string &wordhint);
std::string createScore(std::string path, std::string &word, std::string &n_succ, std::string &n_trials);
void createScoreFile(std::string PLID, std::string path, std::string date);
void moveToDone(std::string type, std::string PLID);
void writeFile(std::string type, std::string PLID, std::string guess);
int searchOnFile(std::string type, std::string PLID, std::string trial, std::string &word, std::string letter, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
int win_play(std::string PLID, std::string trial, std::string letter, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void server_start(std::string word_file, std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void server_play(std::string PLID, std::string trial, std::string letter, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void server_guess(std::string PLID, std::string trial, std::string gword, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void server_quit(std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen);
void udp_server(std::string word_file, std::string GSPort, int v);

#endif