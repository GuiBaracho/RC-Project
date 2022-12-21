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

void connectUDPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res){

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        std::cerr << "client: UDP: socket creation error\n";
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(GSIP.c_str(), GSPort.c_str(), &hints, &res) == -1){
        std::cerr << "client: UDP: getadressinfo error\n";
        exit(EXIT_FAILURE);
    }
}

void connectTCPClient(std::string GSIP, std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res){
    
    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1){
        std::cerr << "client: TCP: socket creation error\n";
        exit(EXIT_FAILURE);
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    if(getaddrinfo(GSIP.c_str(), GSPort.c_str(), &hints, &res) == -1){
        std::cerr << "client: TCP: getadressinfo error\n";
        exit(EXIT_FAILURE);
    }

    if(connect(fd,res->ai_addr,res->ai_addrlen) == -1){
        std::cerr << "client: TCP: connect error\n";
        exit(EXIT_FAILURE);
    }
}


void connectUDPServer(std::string GSPort, int &fd, struct addrinfo &hints, struct addrinfo *&res) {
    int err;
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1){
        std::cerr << "server: UDP: socket creation error\n";
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, GSPort.c_str(), &hints, &res) == -1){
        std::cerr << "server: UDP: getadressinfo error\n";
        exit(EXIT_FAILURE);
    }
    int reuse = 1;
    err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); //SO WE ARE ABLE TO REUSE THE PORT
    if(err == -1){
        std::cerr << "server: UDP: setsockopt error: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
    err = bind(fd, res->ai_addr, res->ai_addrlen);
    if(err == -1){
        std::cerr << "server: UDP: bind error: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
}