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
#include "commands.h"
#include "connections.h"
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
    int fdU;
    std::string GSIP = DEFAULT_IP;
    std::string GSPort = DEFAULT_PORT;
    std::string toString = "";
    std::string argv1, argv2, argv3, argv4, buffer;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hintsU, *resU;
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
            exit(EXIT_FAILURE);
    }

    std::cout << GSIP << "\n";
    std::cout << GSPort << "\n";

    if(connectUDPClient(GSIP, GSPort, fdU, hintsU, resU) == -1){
        _exit();
    }

    while(1){
        std::cin >> buffer;
        std::stringstream ss(buffer);
        std::string command;
        ss >> command;

        if (command == "start" | command == "sg"){
            std::string PLID;
            ss >> PLID;
            start(PLID);
        } else if (command == "play" | command == "pl"){
            play(command);
        } else if (command == "guess" | command == "gw"){
            guess(command);
        } else if (command == "scoreboard" | command == "sb"){
            scoreboard();
        } else if (command == "hint" | command == "h"){
            hint();
        } else if (command == "state" | command == "st"){
            state();
        } else if (command == "quit"){
            quit();
        } else if (command == "exit"){
            _exit();
        } else {
            std::cerr << "player: '" << command << "' is not a valid command\nUsage: [start | sg] [play | pl] [guess | gw] [scoreboard | sb] [hint | h] [hint | h] [quit] [exit]\n";
        }
    }
        
    return 0;
}