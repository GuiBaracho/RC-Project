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
#include <vector>

#include "commands.h"
#include "connections.h"

#define SB "RSB"
#define H "RHL"
#define ST "RST"

#define MAX_HEADER 64

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
    file.close();
}

int receiveTCPFile(int fd, std::string header[4], std::string type){
    int n, max_h = 0, size_h = 0, offset = 0, i = 0;
    std::string header_str, arg;
    char buffer[512];


    max_h = MAX_HEADER;
    while(max_h > 0 && (n = read(fd,buffer + offset,max_h)) > 0){
        max_h -= n;
        offset += n;
    }

    if(n == -1){
        std::cerr << "TCP: read error\n";
        exit(EXIT_FAILURE);
    }

    header_str.append(buffer, 0, n);
    std::cout << "DID U RECEIVE ANYTHING " << header_str << std::endl;

    std::stringstream ss(header_str);

    while(i < 4 && (ss >> arg)){
        header[i] = arg;
        size_h += header[i].length() + 1;
        i++;
    }
    if(i != 4){
        std::cerr << "TCP: Syntax of response incorrect1\n";
        return -1;
    }

    if(header[0] != type){
        std::cerr << "TCP: Syntax of response incorrect2\n";
        return -1;
    }
    

    if(type == SB){
        if(header[1] == "EMPTY"){
            std::cout << "Scoreboard is empty\n";
            return -1;
        }
    } else if(type == H){
        if(header[1] == "NOK"){
            std::cout << "No data available\n";
            return -1;
        }
    } else if(type == ST){
        if(header[1] == "NOK"){
            std::cout << "No data avilable\n";
            return -1;
        }
    }

    // for(int i = 0; i < 4; i++){
    //     ss >> header[i];
    //     if(header[i] == "NOK" || header[i] == "EMPTY"){
    //         return;
    //     }
    //     size_h += header[i].length() + 1;
    // }

    std::ofstream file(header[2]);
    file.write(buffer + size_h, (MAX_HEADER - size_h)*sizeof(char));

    int flength = stoi(header[3]) - ((MAX_HEADER - size_h));
    while((n = read(fd,buffer,512)) > 0){
        file.write(buffer, n*sizeof(char));
        flength -= n;
        if(flength <= 0){
            break;
        }
    }
    if(n == -1){
        std::cerr << "TCP: read error\n";
        remove(header[2].c_str());
        exit(EXIT_FAILURE);
    }
    file.close();
    return 0;
}

void start(std::string &PLID, std::string &word, int fd, struct addrinfo *&res){
    int err;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    int counter = 1;
    std::string nletter, nerror, w;
    std::string msg = "SNG " + PLID + "\n";
    std::string b;
    addrlen = sizeof(addr);

    std::cout << msg <<"\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1){
        std::cout << "start: UDP: sendto error\n";
        exit(EXIT_FAILURE);
    }

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1){
        std::cout << "start: UDP: recvfrom error\n";
        exit(EXIT_FAILURE);
    }

    b.append(buffer, 0, err);
    std::cout << b << "\n";
    std::stringstream ss(b);
    std::string m;
    while(ss >> m){
        if (counter == 1 && m == "ERR" || counter == 2 && m == "ERR") {
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
    std::string b;
    addrlen = sizeof(addr);

    std::cout << msg <<"\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1){
        std::cout << "play: UDP: sendto error\n";
        exit(EXIT_FAILURE);
    }

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1){
        std::cout << "play: UDP: recvfrom error\n";
        exit(EXIT_FAILURE);
    }

    b.append(buffer, 0, err);
    std::cout << b << "\n";
    std::stringstream ss(b);
    std::string m;

    while(ss >> m){
        if (counter == 1 && m == "ERR") {
            std::cout << "Error, missing PLID or invalid syntax" << "\n";
            return;
        }
        else if (counter == 2) {
            status = m;
        }
        else if (counter == 3 && (status == "OK" || status == "NOK")) {
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
        std::cout << "Trial number invalid or repeating the last PLG stored but with a different letter.\n" << "\n";
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
    std::string b;
    addrlen = sizeof(addr);

    std::cout << msg <<"\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1){
        std::cout << "guess: UDP: sendto error\n";
        exit(EXIT_FAILURE);
    }

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1){
        std::cout << "guess: UDP: recvfrom error\n";
        exit(EXIT_FAILURE);
    }

    b.append(buffer, 0, err);
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
    std::string header[4] = {"", "", "", ""};

    connectTCPClient(GSIP, GSPort, fd, hints, res);

    if(write(fd,"GSB\n",4) == -1){
        std::cerr << "scoreboard: TCP: write error SB\n";
        exit(EXIT_FAILURE);
    }

    receiveTCPFile(fd, header, SB);

    char* word;
    readFile(header[2], word);

    freeaddrinfo(res);
    close(fd);
}

void hint(std::string GSIP, std::string GSPort, std::string PLID){
    int fd;
    struct addrinfo hints, *res;
    std::string header[4] = {"", "", "", ""};
    std::string msg;

    connectTCPClient(GSIP, GSPort, fd, hints, res);

    msg = "GHL " + PLID + "\n";

    if(write(fd,msg.c_str(),msg.length()) == -1){
        std::cerr << "hint: TCP: write error\n";
        exit(EXIT_FAILURE);
    }

    std::cout << msg << std::endl;

    receiveTCPFile(fd, header, H);

    if(header[1] == "NOK"){
        std::cout << "No hints for this word\n";
        return;
    }

    std::cout << "File Name: " << header[2] << std::endl;
    std::cout << "File Size: " << header[3] << std::endl;

    freeaddrinfo(res);
    close(fd);
}

void state(std::string GSIP, std::string GSPort, std::string PLID){
    int fd;
    struct addrinfo hints, *res;
    std::string header[4] = {"", "", "", ""};
    std::string msg;

    connectTCPClient(GSIP, GSPort, fd, hints, res);

    msg = "STA " + PLID + "\n";

    if(write(fd,msg.c_str(),msg.length()) == -1){
        std::cerr << "status: TCP: write error SB\n";
        exit(EXIT_FAILURE);
    }

    receiveTCPFile(fd, header, ST);

    if(header[1] == "NOK"){
        std::cerr << "No hints for this word\n";
        return;
    }

    std::cout << "File Name: " << header[2] << std::endl;
    std::cout << "File Size: " << header[3] << std::endl;

    char* word;
    readFile(header[2], word);

    freeaddrinfo(res);
    close(fd);
}

void quit(std::string PLID, int fd, struct addrinfo *&res){
    int err;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];
    std::string msg = "QUT " + PLID + "\n";
    std::string b;
    addrlen = sizeof(addr);

    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1){
        std::cout << "quit: UDP: sendto error\n";
        exit(EXIT_FAILURE);
    }

    err = recvfrom(fd,buffer,128,0, (struct sockaddr*)&addr,&addrlen);
    if(err == -1){
        std::cout << "quit: UDP: recvfrom error\n";
        exit(EXIT_FAILURE);
    }

    b.append(buffer, 0, err);
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