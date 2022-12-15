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

int connectUDP(std::string GSIP, std::string GSPort, struct addrinfo &hints, struct addrinfo *res){
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        std::cout << "UDP socket creation error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(GSIP.c_str(), GSPort.c_str(), &hints, &res) == -1){
        std::cout << "UDP connection error (SE FOR PRECISO TEMOS QUE TRATARO ERRO)\n";
        exit(EXIT_FAILURE);
    }
}