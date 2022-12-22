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

    if(FindLastGame(PLID.c_str(), pgame) == 0){
        std::cout << "There are no current games for this PLID\n";
        send = "NOK";
        return;
    }

    std::ifstream file(pgame);
    if (file.is_open()){
        getline(file, line);
    } else {
        std::cout << "Following file " << pgame << "doesn't exist" << std::endl;
        send = "NOK";
        return;
    }
    file.close();

    std::stringstream ss(line);

    ss >> IMGname;
    ss >> IMGname;

    IMGname = "Dados_GS/" + IMGname;

    std::ifstream image(IMGname);
    if(image.is_open()) {
        file.seekg(0, file.end); //changing offset to the end 
        flength = file.tellg(); //getting the length through offset
        file.seekg(0, file.beg);

        img = new char [flength];
        file.read(img, flength);
    }
    else {
        std::cout << "Following file " << IMGname << "doesn't exist" << std::endl;
        send = "NOK";
        return;
    }
    image.close();
    
    send = "RHL OK " + IMGname + " " + std::to_string(flength) + " " + img;
}



void handleTCP(int fd){
    int n, i = 0;
    char buffer[8];
    std::string arg, send, msg;
    std::cout << "------------------------------------> ANTES READ\n";
    while((n = read(fd,buffer,8)) > 0){
        std::cout << "------------------------------------> ANTES \n";
        msg.append(buffer, 0, n);
        std::cout << "------------------------------------> DEPOIS \n";
    }
    std::cout << "------------------------------------> SAI LOOP \n";
    if(n == -1){
        std::cerr << "server: TCP: read error\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "------------------------------------> DEPOIS READ\n";
    std::cout << "------------------------------------> msg" << msg << "\n";
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

    n = write(fd,send.c_str(),send.length());
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }
    close(fd);
}


void tcp_server(std::string word_file, std::string GSPort) {
    std::cout << "Hello from tcp" << std::endl;
    int fd,newfd, err, n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    std::string toString = "";
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