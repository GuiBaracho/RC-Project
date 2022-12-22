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
#include "commands.h"

void read_server(std::string name_file, std::string &word_file) {
    std::ifstream file;
    std::string data;
    int count = 0;
    file.open(name_file);
    
    if(file.is_open()) {
        while (file >> data) {
            word_file = word_file + data + " ";
            count++;
        }
        int c = count/2;
        std::string nwords = std::to_string(c);
        word_file.insert(0, " ");
        word_file.insert(0, nwords);
        std::cout << word_file << std::endl;

    }
    else {
        std::cout << "Following file " << name_file << " doesn't exist" << std::endl;
    }
    file.close();
}

int FindLastGame(const char* PLID, char* fname){
    struct dirent ** filelist;
    int n_entries, found;
    char dirname[20];

    sprintf(dirname, "GAMES/%s/", PLID);

    n_entries = scandir(dirname, &filelist, 0, alphasort);
    found = 0;

    if(n_entries <= 0){
        return 0;
    } else {
        while(n_entries--){
            if(filelist[n_entries]->d_name[0] != '.'){
                sprintf(fname, "GAMES/%s/%s", PLID, filelist[n_entries]->d_name);
                found = 1;
            }
            free(filelist[n_entries]);
            if(found){
                break;
            }
        }
        free(filelist);
    }
    return found;
}

int server_hint(int fd, std::string PLID, std::string &fname, int &flength, std::string &send){
    int n;
    char* pgame;
    std::string line, IMGname;

    std::string name = "GAMES/GAME_" + PLID + ".txt";

    std::ifstream file(name);
    if (file.is_open()){
        getline(file, line);
    } else {
        std::cout << "Following file " << name << "doesn't exist" << std::endl;
        send = "RHL NOK\n";
        return -1;
    }
    file.close();

    std::stringstream ss(line);

    ss >> IMGname;
    ss >> IMGname;

    fname = "Dados_GS/" + IMGname;

    std::ifstream img(fname);
    if(img.is_open()) {
        img.seekg(0, img.end); //changing offset to the end 
        flength = img.tellg();
    }

    send = "RHL OK " + IMGname + " " + std::to_string(flength) + " ";

    return 0;
}



void handleTCP(int fd){
    int n, err, i = 0, b = 0, flength = 0, offset = 0;
    char buffer[11], buffer2[128];
    std::string arg, send, msg, fname = "";
    while((n = read(fd,buffer + b,11 - b)) > 0){
        msg.append(buffer, 0, n);
        b += n;
    }
    if(n == -1){
        std::cerr << "server: TCP: read error\n";
        exit(EXIT_FAILURE);
    }
    std::stringstream ss(msg);
    if(ss >> arg){
        if(arg == "GSB"){

        } else if(arg == "GHL"){
            if(ss >> arg){
                err = server_hint(fd, arg, fname, flength, send);
            } else {
                send = "ERR\n";
            }
        } else if(arg == "STA"){

        } else {
            send = "ERR\n";
        }
    } else {
        std::cerr << "server: TCP: wrong syntax\n";
        send = "ERR\n";
    }
    std::cout << "Send: " << send << "\n";
    n = write(fd,send.c_str(),send.length());
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }
    if(err == 0){
        std::ifstream file(fname);
        if(file.is_open()){
            while(flength > 0){
                file.seekg(offset, file.beg);
                file.read(buffer2, 128);
                n = write(fd,buffer2,128);
                if(n == -1){
                    std::cerr << "server: TCP: write error\n";
                    exit(EXIT_FAILURE);
                }
                offset += n;
                flength -= n;
            }
        }
        file.close();
        n = write(fd,"\n",1);
        if(n == -1){
            std::cerr << "server: TCP: write error\n";
            exit(EXIT_FAILURE);
        }
    }


    // if(send == "RHL NOK\n" || send == "ERR\n"){
    //     n = write(fd,send.c_str(),send.length());
    //     if(n == -1){
    //         std::cerr << "server: TCP: write error\n";
    //         exit(EXIT_FAILURE);
    //     }
    // } else {
    //     std::ifstream file(fname);
    //     if(file.is_open()) {
    //         file.seekg(0, file.end); //changing offset to the end 
    //         flength = file.tellg();
    //     }
    //     while(fle){

    //     }
    // }

    close(fd);
}


void tcp_server(std::string word_file, std::string GSPort) {
    std::cout << "Hello from tcp" << std::endl;
    int fd, newfd, err, n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    std::string b;

    connectTCPServer(GSPort, fd, hints, res);

    while(1){
        addrlen=sizeof(addr);
        if((newfd = accept(fd,(struct sockaddr*)&addr, &addrlen)) == -1){
            std::cerr << "server: TCP: accept error: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        n = fork();
        if(n == -1){
            std::cerr << "server: TCP: fork error: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        } else if(n == 0){
            handleTCP(newfd);
            exit(EXIT_SUCCESS);
        }
    }
    freeaddrinfo(res);
    close(fd);
}