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

void processFile(int fd, std::string type){
    size_t n, extra;
    char buffer[512];
    std::string msg, fsize, filename;
    std::string toString = "";

    if((n = read(fd,buffer,512)) == -1){
        std::cerr << "TCP read error 2 SB\n";
    }

    msg = toString + buffer;
    std::stringstream ss(msg);

    ss >> filename;
    extra = filename.length() + 1;

    std::ofstream sb(filename);
    
    ss >> fsize;
    extra += fsize.length() + 1;

    if(type == H || type == ST){
        std::cout << "File name: " << filename << std::endl;
        std::cout << "File size: " << fsize << std::endl;
    }

    extra++;
    msg = msg.substr(extra , n - extra);
    sb << msg;

    if(type == SB || type == ST){
        std::cout << msg;
    }

    while((n = read(fd,buffer,512)) != 0){
        if(n == -1){
            remove(filename.c_str());
            std::cerr << "Error reading \n";
            return;
        }
        msg = toString + buffer;
        //msg = msg.substr(0,n);
        sb << msg.substr(0,n);
        if(type == SB || type == ST){
            std::cout << msg;
        }
    }
    std::cout << msg;
    sb.close();

    // char* word;
    // readFile(filename, word);
}

void receiveTCPFile(int fd, std::string header[4]){
    int n, max_h = 0, size_h = 0, offset = 0;
    std::string header_str, arg;
    char buffer[512];

    max_h = MAX_HEADER;
    while(max_h > 0 && (n = read(fd,buffer + offset,max_h)) > 0){
        max_h -= n;
        offset += n;
    }
    if(n == -1){
        std::cerr << "TCP read error\n";
        return;
    }

    header_str.append(buffer, 0, n);

    std::stringstream ss(header_str);
    for(int i = 0; i < 4; i++){
        ss >> header[i];
        if(header[i] == "NOK" || header[i] == "EMPTY"){
            return;
        }
        size_h += header[i].length() + 1;
    }

    std::ofstream file(header[2]);
    file.write(buffer + size_h, (MAX_HEADER - size_h)*sizeof(char));

    while((n = read(fd,buffer,512)) > 0){
        file.write(buffer, n*sizeof(char));
    }
    if(n == -1){
        std::cerr << "TCP read error\n";
        remove(header[2].c_str());
    }
    file.close();


    // int n, msg_size = 0, size, offset = 0;
    // char buffer[1024];
    // std::vector<std::string> response;
    // std::string buffer_str, word; 

    // size = 43; // CMD(3B) + status(max 3B) + Fname(max 24B) + Fsize(max 10B) + spaces(3B) = 43

    // while(size > 0 && (n = read(fd, buffer+offset, size)) > 0) {
    //     size -= n;
    //     offset += n;
        
    // }
    // if(n == -1) { response.push_back("err"); return; }

    // buffer_str.append(buffer, 0, n);

    // std::stringstream ss(buffer_str);

    // for(int i = 0; i < 4; i++) {
    //     getline(ss, word, ' ');
    //     response.push_back(word);
    //     if(word == "NOK") { return; }
    //     msg_size += word.size() + 1;
    // }

    // std::ofstream file(response[2], std::ios::out | std::ios::binary);

    // file.write(buffer + msg_size, (43 - msg_size)*sizeof(char));

    // while((n = read(fd, buffer, 1024)) > 0) {
    //     file.write(buffer, n*sizeof(char));
    // }

    // file.close();
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
    std::string header[4] = {"", "", "", ""};
    std::string toString = "";
    char buffer[128];

    connectTCPClient(GSIP, GSPort, fd, hints, res);

    if(write(fd,"GSB\n",4) == -1){
        std::cerr << "TCP write error SB\n";
    }

    receiveTCPFile(fd, header);

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
        std::cerr << "TCP write error\n";
    }

    receiveTCPFile(fd, header);

    if(header[1] == "NOK"){
        std::cerr << "No hints for this word\n";
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
        std::cerr << "TCP write error SB\n";
    }

    receiveTCPFile(fd, header);

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

