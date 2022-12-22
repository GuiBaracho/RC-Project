#ifndef _CONNECTIONS_
#define _CONNECTIONS_

void connectUDPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res);
void connectTCPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res);
void connectUDPServer(std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res);
void connectTCPServer(std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res);

#endif