#ifndef _CONNECTIONS_
#define _CONNECTIONS_

int connectUDPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res);
int connectTCPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res);
int connectUDPServer(std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res);

#endif