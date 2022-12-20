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
#include <fstream>

#include "commands.h"
#include "connections.h"

int val(std::string command){
    std::stringstream ss;
    int val;
    ss << command;
    ss >> val;
    return val;
}

void readFile(std::string name_file, char *&word_file) {
    std::string data;
    std::ifstream file;
    file.open(name_file);
    if(file.is_open()) {
        file.seekg(0, file.end); //changing offset to the end 
        int flength = file.tellg(); //getting the length through offset
        file.seekg(0, file.beg);

        word_file = new char [flength];
        file.read(word_file, flength);
        std::cout << word_file << std::endl;
    }
    else {
        std::cout << "Following file " << name_file << "doesn't exist" << std::endl;
    }
}

void start(std::string &PLID, std::string &word, int fd, struct addrinfo *&res){
    int err;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    int counter = 1;
    std::string nletter, nerror, w;
    std::string msg = "SNG " + PLID + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    std::cout << msg <<"\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";

    std::string b = toString + buffer;
    std::cout << b << "\n";
    std::stringstream ss(b);
    std::string m;
    while(ss >> m){
        if (counter == 1 && m == "ERR") {
            std::cout << "Syntax of SNG incorrect or PLID invalid" << "\n";
            PLID = "-1";
            return;
        }
        else if (counter == 2 && m == "NOK") {
            std::cout << "There is already a game ongoing" << "\n";
            return;
        }
        else if (counter == 3) {
            std::cout << "Number of letters" << "\n";
            nletter = m;
        }
        else if (counter == 4) {
            nerror = m;
            std::cout << "Number of errors" << "\n";
        }
        counter +=1; 
    }
    int num_letters = val(nletter);
    std::cout << num_letters << "\n";
    for (int i = 0; i < num_letters; i++) {
        w = w + " _";
    }
    word = w;
    
    std::cout << "New game started. Guess " << nletter << " letter word (Max " << nerror << " errors):"<< word << "\n";
}

void play(std::string PLID, char letter, int &trial, std::string &word, int fd, struct addrinfo *&res){
    int err, n_occurrences;
    int counter = 1;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    std::string status;
    std::string ntrial = std::to_string(trial+1);
    std::string msg = "PLG " + PLID + " " + letter + " " + ntrial + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    std::cout << msg <<"\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";

    std::string b = toString + buffer;
    std::cout << b << "\n";
    std::stringstream ss(b);
    std::string m;

    while(ss >> m){
        if (counter == 1 && m == "ERR") {
            std::cout << "Error" << "\n";
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
            if (n_occurrences > 0) {
                int pos = val(m);
                pos = (pos*2)-1;
                word[pos] = letter;
            }
            else {
                break;
            }
            n_occurrences--;
        } 
        counter++; 
    }
    if (status == "OK") {
        std::cout << "Word:" << word << "\n";
        return;
    }
    else if (status == "WIN") {
        int wordl = word.length();
        for (int i = 0; i < wordl; i++) {
            if (word[i] == '_') {
                word[i] = letter;
            }
        }
        std::cout << "Well done, you guessed: " << word << "\n";
        trial = 0;
        return;
    }
    else if (status == "DUP") {
        std::cout << "Letter " << letter << " was sent in a previous trial." << "\n";
        return;
    }
    else if (status == "NOK") {
        std::cout << "Letter " << letter << " is not part of the word." << "\n";
        return;
    }
    else if (status == "OVR") {
        std::cout << "Letter " << letter << " is not part of the word. No more attempts, game over." << "\n";
        trial = 0;
        return;
    }
    else if (status == "INV") {
        std::cout << "Trial number invalid or repeating the last PLG stored but with a different letter." << "\n";
        return;
    }
    else if (status == "ERR") {
        std::cout << "Syntax of RLG incorrect or PLID invalid or there is no game ongoing." << "\n";
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
    std::string ntrial = std::to_string(trial+1);
    std::string msg = "PWG " + PLID + " " + gword + " " + ntrial + "\n";
    std::string toString = "";
    addrlen = sizeof(addr);

    std::cout << msg <<"\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1) std::cout << "Deu erro1\n";

    std::string b = toString + buffer;
    std::cout << b << "\n";
     
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
        std::cout << "Well done, you guessed: " << gword << "\n";
        trial = 0;
        return;
    }
    else if (status == "DUP") {
        std::cout << "Word " << gword << " was sent in a previous trial." << "\n";
        return;
    }
    else if (status == "NOK") {
        std::cout << "Word " << gword << " is not correct." << "\n";
        return;
    }
    else if (status == "OVR") {
        std::cout << "Word " << gword << " is not correct. No more attempts, game over." << "\n";
        trial = 0;
        return;
    }
    else if (status == "INV") {
        std::cout << "Trial number invalid or repeating the last PWG stored but with a different word." << "\n";
        return;
    }
    else if (status == "ERR") {
        std::cout << "Syntax of PWG incorrect or PLID invalid or there is no game ongoing." << "\n";
        return;
    }
}

void scoreboard(std::string GSIP, std::string GSPort){
    int fd;
    size_t n, extra;
    struct addrinfo hints, *res;
    std::string toString = "";
    char buffer[128];

    connectTCPClient(GSIP, GSPort, fd, hints, res);

    if(write(fd,"GSB\n",4) == -1){
        std::cerr << "TCP write error SB\n";
    }
    if(read(fd,buffer,128) == -1){
        std::cerr << "TCP read error SB\n";
    }

    std::string msg = toString + buffer;
    std::stringstream ss(msg);
    std::string m, filename;

    ss >> m;
    ss >> m;

    if(m == "EMPTY"){
        std::cout << "Scoreboard is empty\n";

    } else if (m == "OK") {
        if((n = read(fd,buffer,128)) == -1){
            std::cerr << "TCP read error 2 SB\n";
        }

        msg = toString + buffer;
        std::stringstream ss(msg);

        ss >> filename;

        std::ofstream sb(filename);

        extra = filename.length() + 1;
        ss >> m;
        extra += m.length() + 1;

        sb << msg.substr(extra, n - extra);

        while((n = read(fd,buffer,128)) != 0){

            if(n == -1){
                remove(filename.c_str());
                return;
            }
            msg = toString + buffer;
            sb << msg.substr(0,n);
        }

        sb.close();

        
        
    }

    freeaddrinfo(res);
    close(fd);
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
    std::cout << b << "\n";
    std::stringstream ss(b);
    std::string m;
    while(ss >> m){
        if (m == "OK") {
            std::cout << "Ongoing game quit" << "\n";
            return;
        }
        else if (m == "NOK") {
            std::cout << "There was no ongoing game" << "\n";
            return;
        }
        else if (m == "ERR") {
            std::cout << "There was an error" << "\n";
            return;
        }
    }
}

void _exit(){
    exit(EXIT_SUCCESS);
}

