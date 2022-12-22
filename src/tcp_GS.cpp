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

void server_hint(int fd, std::string PLID, std::string &send){
    int n, flength;
    char* pgame, *img;
    std::string line, IMGname;

    send = "RHL ";

    std::string name = "./GAMES/GAME_" + PLID + ".txt";

    std::ifstream file(name);
    if (file.is_open()){
        getline(file, line);
    } else {
        std::cout << "Following file " << name << "doesn't exist" << std::endl;
        send = send + "NOK\n";
        return;
    }
    file.close();

    std::stringstream ss(line);

    ss >> IMGname;
    ss >> IMGname;

    std::string iname = "./Dados_GS/" + IMGname;

    std::ifstream image(iname);
    if(image.is_open()) {
        image.seekg(0, image.end); //changing offset to the end 
        flength = image.tellg(); //getting the length through offset
        image.seekg(0, file.beg);
        img = new char [flength];
        image.read(img, flength);
    }
    else {
        send = send + "NOK\n";
        return;
    }
    image.close();
    send = "RHL OK " + IMGname + " " + std::to_string(flength) + " " + img + "\n";
}



void handleTCP(int fd){
    int n, i = 0, b = 0;
    char buffer[11];
    std::string arg, send, msg;
    std::cout << fd << "------------------------------------> ANTES READ\n";
    while((n = read(fd,buffer + b,11 - b)) > 0){

        std::cout << n << "------------------------------------> ANTES \n";
        msg.append(buffer, 0, n);
        b += n;
        std::cout << "------------------------------------> DEPOIS \n";
    }
    std::cout << "------------------------------------> SAI LOOP \n";
    if(n == -1){
        std::cerr << "server: TCP: read error\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "------------------------------------> msg " << msg << "\n";
    std::stringstream ss(msg);
    if(ss >> arg){
        if(arg == "GSB"){

        } else if(arg == "GHL"){
            if(ss >> arg){
                server_hint(fd, arg, send);
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
    std::cout << send << "------------------------------------> SEND\n";
    n = write(fd,send.c_str(),send.length());
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }

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
        std::cout << "------------------------------------> TESTE1\n";
        n = fork();
        if(n == -1){
            std::cerr << "server: TCP: fork error: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        } else if(n == 0){
            std::cout << "------------------------------------> FILHO\n";
            handleTCP(newfd);
            std::cout << "------------------------------------> Deu merda\n";
            exit(EXIT_SUCCESS);
        }
        std::cout << "------------------------------------> PAI\n";
    }
    freeaddrinfo(res);
    close(fd);
}