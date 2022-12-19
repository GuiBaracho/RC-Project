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
#include "connections.h"

int val(std::string command){
    std::stringstream ss;
    int val;
    ss << command;
    ss >> val;
    return val;
}

void start(std::string PLID, std::string &word, int fd, struct addrinfo *&res){
    int err;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    int counter = 1;
    std::string nletter, nerror;
    std::string msg = "SNG " + PLID + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    std::cout << msg <<std::endl;
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";

    std::string b = toString + buffer;
    std::cout << b << std::endl;
    std::stringstream ss(b);
    std::string m;

    while(ss >> m){
        if (counter == 1 && m == "ERR") {
            std::cout << "Syntax of SNG incorrect or PLID invalid" << std::endl;
            return;
        }
        else if (counter == 2 && m == "NOK") {
            std::cout << "There is already a game ongoing" << std::endl;
            return;
        }
        else if (counter == 3) {
            nletter = m;
        }
        else if (counter == 4) {
            nerror = m;
        }
        counter +=1; 
    }
    int num_letters = val(nletter);
    for (int i = 0; i < num_letters; i++) {
        word += " _";
    }
    std::cout << "New game started. Guess " << nletter << " letter word (Max " << nerror << " errors):"<< word << std::endl;
}

void play(std::string PLID, std::string letter, int &trial, std::string &word, int fd, struct addrinfo *&res){
    int err, n_occurrences;
    int counter = 1;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    std::string status;
    std::string ntrial = std::to_string(trial);
    std::string msg = "PLG " + PLID + " " + letter + " " + ntrial + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    std::cout << msg <<std::endl;
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";

    std::string b = toString + buffer;
    std::cout << b << std::endl;
    std::stringstream ss(b);
    std::string m;

    while(ss >> m){
        if (counter == 1 && m == "ERR") {
            std::cout << "Error" << std::endl;
            return;
        }
        else if (counter == 2) {
            status = m;
        }
        else if (counter == 3 && (status == "OK" || status == "NOK") ) {
            trial = val(m);
        }
        else if (counter == 4 && status == "OK") {
            n_occurrences = val(m);
        }
        else if (counter > 4 && status == "OK") {
            for (int i = 0; i < n_occurrences; i++) {
                int pos = val(m);
                pos = (pos*2)-1;
                word[pos] = letter.at(0);
            }
        } 
        counter++; 
    }
    if (status == "OK") {
        std::cout << "Word:" << word << std::endl;
        trial++;
        return;
    }
    else if (status == "WIN") {
        std::cout << "Well done, you guessed: " << word << std::endl;
        trial = 1;
        //still need to code adding the last letter guessed to the word
        return;
    }
    else if (status == "DUP") {
        std::cout << "Letter " << letter << " was sent in a previous trial." << std::endl;
        return;
    }
    else if (status == "NOK") {
        std::cout << "Letter " << letter << " is not part of the word." << std::endl;
        trial++;
        return;
    }
    else if (status == "OVR") {
        std::cout << "Letter " << letter << " is not part of the word. No more attempts, game over." << std::endl;
        trial = 1;
        return;
    }
    else if (status == "INV") {
        std::cout << "Trial number not invalid or repeating the last PLG stored but with a different letter." << std::endl;
        return;
    }
    else if (status == "ERR") {
        std::cout << "Syntax of RLG incorrect or PLID invalid or there is no game ongoing." << std::endl;
        return;
    }
    
}

void guess(std::string PLID, std::string gword, int &trial, int fd, struct addrinfo *&res){
    int err;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    int counter = 1;
    std::string status;
    std::string ntrial = std::to_string(trial);
    std::string msg = "PWG " + PLID + " " + gword + " " + ntrial + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    std::cout << msg <<std::endl;
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";

    std::string b = toString + buffer;
    std::cout << b << std::endl;
     
    std::stringstream ss(b);
    std::string m;
    while(ss >> m){
        if (counter == 2) {
            status = m;
        }
        else if (counter == 3 && (status == "OK" || status == "NOK")) {
            trial = val(m);
        } 
        counter++; 
    }
    if (status == "WIN") {
        std::cout << "Well done, you guessed: " << gword << std::endl;
        return;
    }
    else if (status == "DUP") {
        std::cout << "Word " << gword << " was sent in a previous trial." << std::endl;
        return;
    }
    else if (status == "NOK") {
        std::cout << "Word " << gword << " is not correct." << std::endl;
        trial++;
        return;
    }
    else if (status == "OVR") {
        std::cout << "Word " << gword << " is not correct. No more attempts, game over." << std::endl;
        trial = 1;
        return;
    }
    else if (status == "INV") {
        std::cout << "Trial number not invalid or repeating the last PWG stored but with a different word." << std::endl;
        return;
    }
    else if (status == "ERR") {
        std::cout << "Syntax of PWG incorrect or PLID invalid or there is no game ongoing." << std::endl;
        return;
    }
}

void scoreboard(std::string GSIP, std::string GSPort){
    std::cout << "scoreboard" << "\n";
    int fd;
    socklen_t addrlen;
    struct addrinfo hints, *res;

    if(connectTCPClient(GSIP, GSPort, fd, hints, res) == -1){
        
    }


    if(write(fd,"Hello!\n",7) == -1){
        std::cerr << "TCP write error SB\n";
    }
    if(n==-1)/*error*/exit(1);
    n=read(fd,buffer,128);
    if(n==-1)/*error*/exit(1);
}

void hint(){
    std::cout << "hint" << "\n";
}

void state(){
    std::cout << "state" << "\n";
}

void quit(std::string PLID, int fd, struct addrinfo *&res){
    int err;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    std::string msg = "QUT " + PLID + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";
    std::string b = toString + buffer;
    std::cout << b << std::endl;
    std::stringstream ss(b);
    std::string m;
    while(ss >> m){
        if (m == "OK") {
            std::cout << "Ongoing game quit" << std::endl;
            return;
        }
        else if (m == "NOK") {
            std::cout << "There was no ongoing game" << std::endl;
            return;
        }
        else if (m == "ERR") {
            std::cout << "There was an error" << std::endl;
            return;
        }
    }
}

void _exit(){
    exit(EXIT_SUCCESS);
}

