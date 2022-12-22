#ifndef _TCP_GS_
#define _TCP_GS_

struct SCORELIST
{
    int n_scores;
    int score[10];
    std::string PLID[10];
    std::string word[10];
    int n_succ[10];
    int n_tot[10];
};

void create_stateFIN(std::string PLID, std::string name);
int lastGame(int fd, std::string PLID);
int create_state(int fd, std::string PLID);
int FindLastGame(const char* PLID, char* fname);
void server_hint(int fd, std::string PLID, std::string &send);
void handleTCP(int fd, SCORELIST* list);
void tcp_server(SCORELIST* list, std::string GSPort, int v);

#endif