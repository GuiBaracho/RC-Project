#ifndef _COMMANDS_
#define _COMMANDS_

void start(std::string &PLID, std::string &word, int fd, struct addrinfo *&res);
void play(std::string PLID, char letter, int &trial, std::string &word, int fd, struct addrinfo *&res);
void guess(std::string PLID, std::string gword, int &trial, int fd, struct addrinfo *&res);
void scoreboard(std::string GSIP, std::string GSPort);
void hint(std::string GSIP, std::string GSPort, std::string PLID);
void state(std::string GSIP, std::string GSPort, std::string PLID);
void quit(std::string PLID, int fd, struct addrinfo *&res);
void _exit();
int val(std::string command);
void readFile(std::string name_file, char *&word_file);

#endif