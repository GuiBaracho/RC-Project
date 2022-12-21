#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <sstream>

#include "connections.h"

int connectUDPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res){

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        std::cerr << "UDP socket creation error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        return -1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(GSIP.c_str(), GSPort.c_str(), &hints, &res) == -1){
        std::cerr << "UDP getadressinfo error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        return -1;
    }

    return 0;
}

int connectTCPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res){
    
    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1){
        std::cerr << "TCP socket creation error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        return -1;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    if(getaddrinfo(GSIP.c_str(), GSPort.c_str(), &hints, &res) == -1){
        std::cerr << "TCP getadressinfo error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        return -1;
    }

    if(connect(fd,res->ai_addr,res->ai_addrlen) == -1){
        std::cerr << "TCP connection error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        return -1;
    }
    return 0;
}


int connectUDPServer(std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res) {
    int err;
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        std::cerr << "UDP socket creation error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        return -1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, GSPort.c_str(), &hints, &res) == -1){
        std::cerr << "UDP getadressinfo error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        return -1;
    }
    int reuse = 1;
    err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); //SO WE ARE ABLE TO REUSE THE PORT
    err = bind(fd, res->ai_addr, res->ai_addrlen);
    if(err == -1) std::cout << "UDP bind error\n" << strerror(errno);

    return 0;
}