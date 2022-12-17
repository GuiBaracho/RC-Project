#ifndef _COMMANDS_
#define _COMMANDS_

void start(std::string PLID, int fd, struct addrinfo *&res);
void play(std::string letter);
void guess(std::string word);
void scoreboard();
void hint();
void state();
void quit();
void _exit();

#endif