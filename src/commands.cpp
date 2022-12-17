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

#include "commands.h"

// int val(std::string command){
//     std::stringstream ss;
//     int val;
//     ss << command;
//     ss >> val;
//     return val;
// }

void start(std::string PLID, int fd, struct addrinfo *&res){
    int err;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    std::string msg = "SNG " + PLID + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    
    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";

    std::string b = toString + buffer;
    std::cout << b << "\n";
    std::stringstream ss(b);
    std::string m;
    while(ss >> m){
        std::cout << m << "\n";
    }
}

void play(std::string letter){
    std::cout << "play" << "\n";
}

void guess(std::string word){
    std::cout << "guess" << "\n";
}
void scoreboard(){
    std::cout << "scoreboard" << "\n";
}

void hint(){
    std::cout << "hint" << "\n";
}

void state(){
    std::cout << "state" << "\n";
}

void quit(){
    std::cout << "quit" << "\n";
}

void _exit(){
    exit(EXIT_SUCCESS);
}