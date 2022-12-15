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

void start(std::string PLID){
    std::cout << "start" << "\n";
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

void exit(){
    exit(EXIT_SUCCESS);
}