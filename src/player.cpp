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

int main(int argc, char **argv){
    int fd, trial;
    std::string GSIP = DEFAULT_IP;
    std::string GSPort = DEFAULT_PORT;
    std::string argv1, argv2, argv3, argv4, buffer;
    std::string word, PLID;
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
            exit(EXIT_FAILURE);
    }

    connectUDPClient(GSIP, GSPort, fd, hints, res);
    
    while(1){
        std::getline (std::cin, buffer);
        std::stringstream ss(buffer);
        std::string command;
        ss >> command;

        if (command == "start" | command == "sg"){
            if(ss >> command) {
                start(command, word, fd, res);
                if (command != "-1") {
                    if(PLID != command){
                        PLID = command;
                        trial = 0;
                    }
                }
            }
        } else if (command == "play" | command == "pl"){
            char command;
            if (ss >> command) {
                play(PLID, command, trial, word, fd, res);
            }
        } else if (command == "guess" | command == "gw"){
            if (ss >> command) {
                guess(PLID, command, trial, fd, res);
            }
        } else if (command == "scoreboard" | command == "sb"){
            scoreboard(GSIP, GSPort);
        } else if (command == "hint" | command == "h"){
            hint(GSIP, GSPort, PLID);
        } else if (command == "state" | command == "st"){
            state(GSIP, GSPort, PLID);
        } else if (command == "quit"){
            quit(PLID, fd, res);
            trial = 0;
            word.clear();
        } else if (command == "exit"){
            quit(PLID, fd, res);
            freeaddrinfo(res);
            close(fd);
            _exit();
        } else {
            std::cerr << "player: '" << command << "' is not a valid command\nUsage: [start | sg] [play | pl] [guess | gw] [scoreboard | sb] [hint | h] [hint | h] [quit] [exit]\n";
        }
    }
        
    return 0;
}