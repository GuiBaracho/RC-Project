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
#include <dirent.h>


#include "connections.h"

int value(std::string command){
    std::stringstream ss;
    int val;
    ss << command;
    ss >> val;
    return val;
}

int searchONGame(std::string PLID, std::string gword) {
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
        new_file << gword << std::endl;
        new_file.close();
        return 0;
    }
    return -1;

}

void error_message(std::string command, std::string PLID, int v_mode, int &fd, struct addrinfo &hints, struct addrinfo *&res) {
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
    err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
    if(err == -1) std::cout << "Deu erro\n";

}

void server_start(std::string word_file, std::string PLID, int &fd, struct addrinfo &hints, struct addrinfo *&res) {

    std::string word, gword;
    int err, nword, num, max_errors, n;
    int count = 1;
    time_t t;
    srand((unsigned) time(&t));
        std::stringstream ss(word_file);

        ss >> word;
        nword = value(word);
        num = (rand() % nword) + 1;
        while(ss >> word) {
            if (count == num) {
                gword = word;
            }
            count++;
    }

    n = searchONGame(PLID, gword);
    if (n == 1) {
        std::string msg = "RSG NOK";

        err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
        if(err == -1) std::cout << "Deu erro\n";
    } else if (n == 0) {
        
        std::cout << gword << std::endl;
        int word_size = gword.length();
        if (word_size <= 6) {
            max_errors = 7;
        } else if (word_size >= 7 && word_size <= 10) {
            max_errors = 8;
        } else if (word_size > 10) {
            max_errors = 9;
        }

        std::string msg = "RSG OK " + std::to_string(word_size) + std::to_string(max_errors);

        err = sendto(fd,msg.c_str(),msg.length(),0,res->ai_addr,res->ai_addrlen);
        if(err == -1) std::cout << "Deu erro\n";

        std::cout << msg << "\n";
    }
    
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
                if (type != "SNG" || type != "PLG" || type != "PWG" || type != "QUT") {
                    error_message(type, PLID, v_mode, fd, hints, res);
                    break;
                }
            } else if (counter == 2) {
                PLID = m;
                size = PLID.length();
                if (size != 6) {
                    error_message(type, PLID, v_mode, fd, hints, res);
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
                    error_message(type, PLID, v_mode, fd, hints, res);
                    break;
                }
            }
            counter++; 
        }
        if (type == "SNG") {
            server_start(word_file, PLID, fd, hints, res);
        } else if (type == "PLG") {
            /* code */
        } else if (type == "PWG") {

        } else if (type == "QUT") {

        }
    }

    freeaddrinfo(res);
    close(fd);
}


