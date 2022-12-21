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

#include "connections.h"

int value(std::string command){
    std::stringstream ss;
    int val;
    ss << command;
    ss >> val;
    return val;
}

int createFileONGame(std::string PLID, std::string gword) {
    std::ifstream file;
    std::string path = "./ONGOING_GAMES/" + PLID + ".txt";
    int cont;

    file.open(path);
    if(file.is_open()) {
        std::string data;
        while (file >> data) {
            cont++;
        }
        if (cont == 1) {
            file.close();
            return 0;
        } 
        file.close();
        return 1;
    }
    else {
        std::ofstream new_file;
        std::string name = "./ONGOING_GAMES/" + PLID + ".txt";
        new_file.open(name);
        if (new_file.is_open()) {
            new_file << gword << "\n";
        } else {
            std::cout << "why is not opened\n";
        }
        new_file.close();
        return 0;
    }
    return -1;

}

void error_message(std::string command, std::string PLID, int v_mode, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    int err;
    std::string msg;
    if (command == "SNG" || command == "PLG" || command == "PWG" || command == "QUT") {
        msg = "RSG ERR\n";
    } else if (command == "PLG") {
        msg = "RLG ERR\n";
    } else if (command == "PWG") {
        msg = "RWG ERR\n";
    } else if (command == "QUT") {
        msg = "RQT ERR\n";
    } else {
        msg = "ERR\n";
        if (v_mode = 1) {
            std::cout << "The following incorrect type of request " << command << " was received\n";
        }
    }
    if (msg != "ERR\n") {
        if (v_mode = 1) {
            std::cout << "The following type of request " << command << " was received from PLID " << PLID << " with syntax errors\n";
        }
    }
    err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
    if(err == -1) std::cout << "Deu erro\n";
}


void server_start(std::string word_file, std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {

    std::string word, gword;
    int err, num, nword, max_errors, n;
    int count = 1;
    time_t t;
    srand((unsigned) time(&t));
    std::stringstream ss(word_file);

    ss >> word;
    std::cout << "NUMBER OF WORDS " << word << std::endl;
    nword = value(word);
    std::cout << nword << std::endl;
    num = (rand() % nword) + 1;
    std::cout << "RANDOM NUMBER " << num << std::endl;
    while(ss >> word) {
        if (count == num) {
            gword = word;
        }
        count++;
    }
    std::cout << gword << std::endl;

    n = createFileONGame(PLID, gword);
    if (n == 1) {
        std::string msg = "RSG NOK\n";
        
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) std::cout << "Deu erro\n";
        std::cout << msg << std::endl;
    } else if (n == 0) {
        
        int word_size = gword.length();
        if (word_size <= 6) {
            max_errors = 7;
        } else if (word_size >= 7 && word_size <= 10) {
            max_errors = 8;
        } else if (word_size > 10) {
            max_errors = 9;
        }

        std::string msg = "RSG OK " + std::to_string(word_size) + " " + std::to_string(max_errors) + "\n";

        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) std::cout << "Deu erro\n";

        std::cout << msg << "\n";
    }
}

void searchOnFile(std::string PLID, std::string trial, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    int err, cont;
    std::ifstream file;
    int ntrial = value(trial);
    std::string path = "./ONGOING_GAMES/" + PLID + ".txt";
    file.open(path);
    if(file.is_open()) {
        std::string data;
        while (file >> data) {
            cont++;
        }
        file.close();
    }
    else { 
        std::string msg = "RLG ERR\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
    }
}


void server_play(std::string PLID, std::string trial, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
}


void server_guess() {
}


void server_quit(std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    int err;
    std::ifstream file;
    std::string path = "./ONGOING_GAMES/" + PLID + ".txt";
    file.open(path);
    if(file.is_open()) {
        file.close();
        std::string new_path = "./FINISHED_GAMES/" + PLID + ".txt";
        rename(path.c_str(), new_path.c_str());
    } else {
        std::string msg = "RQT NOK\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        return;
    }
    std::string msg = "RQT OK\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
}


void udp_server(std::string word_file, std::string GSPort, int v_mode) {
    std::cout << "Hello from udp" << std::endl;
    int fd, err;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    std::string toString = "";
    
    if(connectUDPServer(GSPort, fd, hints, res) == -1){
        std::cout << "UDP server didn't connect\n";
    }

    while(1) {

        addrlen = sizeof(addr);
        err = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if(err == -1) {
            std::cout << "Deu erro1\n" << strerror(errno);
        }
        std::string b = toString + buffer;
        std::cout << b << "\n";
        std::stringstream ss(b);
        std::string m, type, PLID, letter, word, trial;
        int size;
        int counter = 1;
        
        while(ss >> m) {
            if (counter == 1) {
                type = m;
                if (!(type == "SNG" || type == "PLG" || type == "PWG" || type == "QUT")) {
                    std::cout << type << "is wrong?" << std::endl;
                    error_message(type, PLID, v_mode, fd, addr, addrlen);
                    break;
                }
            } else if (counter == 2) {
                PLID = m;
                size = PLID.length();
               
                if (size != 6) {
                    std::cout << PLID << " " << size << std::endl;
                    error_message(type, PLID, v_mode, fd, addr, addrlen);
                    break;
                }
            } else if (counter > 3) {
                if (type == "PLG" || type == "PWG") {
                    if (counter == 3 || type == "PLG") {
                        letter = m;
                    } else if (counter == 3 || type == "PWG") {
                        word = m;
                    } else if (counter == 4) {
                        trial = m;
                        break;
                    }
                }
                else {
                    std::cout << "if it's here then fuck you" << std::endl;
                    error_message(type, PLID, v_mode, fd, addr, addrlen);
                    break;
                }
            }
            counter++; 
        }
        if (type == "SNG") {
            server_start(word_file, PLID, fd, addr, addrlen);
        } else if (type == "PLG") {
            server_play(PLID, trial, fd, addr, addrlen);
        } else if (type == "PWG") {
            server_guess();
        } else if (type == "QUT") {
            server_quit(PLID, fd, addr, addrlen);
        }
    }

    freeaddrinfo(res);
    close(fd);
}