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

#include "process_input.h"
//using namespace std;

#define DEFAULT_IP "localhost"
#define DEFAULT_PORT "58018"

int val(std::string command){
    std::stringstream ss;
    int val;
    ss << command;
    ss >> val;
    return val;
}

int main(int argc, char **argv){
    int fd, errcode;
    std::string GSIP = DEFAULT_IP;
    std::string GSPort = DEFAULT_PORT;
    std::string toString = "";
    std::string argv1, argv2, argv3, argv4, buffer;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;

    switch (argc) {
        case 1:
            break;

        case 2:
            processInput2(argv);
            
        case 3:
            processInput3(GSIP, GSPort, argv);
            break;
            
        case 4:
            processInput4(argv);
            break;
            
        case 5:
            processInput5(GSIP, GSPort, argv);
            break;

        default:
            std::cerr << "player: error: too many arguments\n";
            exit(0);
    }

    std::cout << GSIP << "\n";
    std::cout << GSPort << "\n";

    while(1){
        std::cin >> buffer;
        std::stringstream ss(buffer);
        std::string command;
        ss >> command;

        switch(val(command)){

        }
    }
        
    return 0;
}