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

#include "tcp_GS.h"
#include "connections.h"
#include "commands.h"

#define TITLE "-------------------------------- TOP 10 SCORES --------------------------------"
#define SB_HEADER "    SCORE PLAYER     WORD                             GOOD TRIALS  TOTAL TRIALS"

std::string addSpaces(int n){
    std::cout << "Fucn: addSpaces\n";
    std::string space;
    for(int i = 0; i < n; i++){
        space += " ";
    }
    return space;
}

std::string place(int n){
    std::cout << "Fucn: place\n";
    if(n < 10){
        std::string place = " " + std::to_string(n);
        return place;
    } else if(n < 100){
        std::string place = "  " + std::to_string(n);
        return place;
    }
    return std::to_string(n);
}

void write_TCP(int fd, std::string msg){
    int n;
    n = write(fd,msg.c_str(),msg.length());
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }
}

void read_TCP(int fd, std::string &arg, int b_size){
    int n, b = 0;
    char buffer[b_size];
    std::string msg;
    msg.clear();
    while(b < b_size && (n = read(fd,buffer,b_size - b)) != -1){
        msg.append(buffer, 0, n);
        b += n;
    }
    if(n == -1){
        std::cerr << "server: TCP: read error\n";
        exit(EXIT_FAILURE);
    }
    std::stringstream ss(msg);
    ss >> arg;
}

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

int FindTopScores(SCORELIST *list){
    struct dirent **filelist;
    int n_entries, i_file;
    char fname[50];
    FILE *fp;
    char PLID[10], word[40];
    std::string s = "";

    n_entries = scandir("SCORES/", &filelist, 0, alphasort);

    i_file = 0;
    if(n_entries < 0){
        return 0;
    } else {
        while(n_entries--){
            if(filelist[n_entries]->d_name[0] != '.'){
                sprintf(fname, "SCORES/%s", filelist[n_entries]->d_name);
                fp = fopen(fname, "r");
                if(fp != NULL){
                    fscanf(fp, "%d %s %s %d %d", &list->score[i_file], PLID, word, &list->n_succ[i_file], &list->n_tot[i_file]);
                    list->PLID[i_file] = s + PLID;
                    list->word[i_file] = s + word;
                    fclose(fp);
                    ++i_file;
                }
            }
            free(filelist[n_entries]);
            if(i_file == 10){
                break;
            }
        }
        free(filelist);
    }
    list->n_scores = i_file;
    return i_file;
}

int server_hint(int fd, std::string PLID, std::string &fname, int &flength, std::string &send){
    int n, offset = 0;
    char* pgame, buffer[128];
    std::string line, IMGname;

    std::string name = "GAMES/GAME_" + PLID + ".txt";

    std::ifstream file(name);
    if (file.is_open()){
        getline(file, line);
    } else {
        std::cout << "Following file " << name << "doesn't exist" << std::endl;
        send = "RHL NOK\n";
        n = write(fd,send.c_str(),send.length());
        if(n == -1){
            std::cerr << "server: TCP: write error\n";
            exit(EXIT_FAILURE);
        }
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
    img.close();
    send = "RHL OK " + IMGname + " " + std::to_string(flength) + " ";

    n = write(fd,send.c_str(),send.length());
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }
    std::ifstream msg(fname);
    if(msg.is_open()){
        while(flength > 0){
            msg.seekg(offset, msg.beg);
            msg.read(buffer, 128);
            n = write(fd,buffer,128);
            if(n == -1){
                std::cerr << "server: TCP: write error\n";
                exit(EXIT_FAILURE);
            }
            offset += n;
            flength -= n;
        }
    }
    msg.close();
    n = write(fd,"\n",1);
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }

    return 0;
}

int server_scoreboard(int fd, SCORELIST* list){
    int n_scores, n, line;
    std::string msg;

    n_scores = FindTopScores(list);

    if(n_scores == 0){
        write_TCP(fd, "RSB EMPTY\n");
        return -1;
    } else {
        msg = "RSB OK TOPSCORES.txt " + std::to_string(80*n_scores + 1) + " " + TITLE+ "\n" + SB_HEADER + "\n";
        write_TCP(fd, msg);
        std::cout << "n: " << n_scores << "\n";
        for(int i = 0; i < n_scores; i++){
            //std::cout << "n:1 "<<place(i+1)<< "\n";
            msg = place(i+1);
            //std::cout << "n:2 " << "\n";
            msg += " - ";
            //std::cout << "n:3 " <<std::to_string(list->score[i])<< "\n";
            msg += place(list->score[i]);
            //std::cout << "n:4 " << "\n";
            msg += "  ";
            //std::cout << "n:5 " <<list->PLID[i] << "\n";
            msg += list->PLID[i];
            //std::cout << "n:6 " << "\n";
            msg += "  ";
            //std::cout << "n:7 " <<list->word[i] << "\n";
            msg += list->word[i];
            //std::cout << "n:8 " << "\n";
            msg += addSpaces(40 - list->word[i].length());
            //std::cout << "n:9 " << "\n";
            msg += place(list->n_succ[i]);
            //std::cout << "n:10 " << "\n";
            msg += "             ";
            //std::cout << "n:11 " << "\n";
            msg += place(list->n_succ[i]) + "\n";
            std::cout << "n: "<< msg << "\n";
            write_TCP(fd, msg);
        }
    }
    return 0;
}

void handleTCP(int fd, SCORELIST* list){
    int n, err, i = 0, b = 0, flength = 0, offset = 0;
    char buffer[11], buffer2[128];
    std::string arg, send = "ERR\n", msg, fname = "";
    
    read_TCP(fd, arg, 3);

    if(arg == "GSB"){
        server_scoreboard(fd, list);
    }else if(arg == "GHL"){
        read_TCP(fd, arg, 7);
        server_hint(fd, arg, fname, flength, send);
    }else if(arg == "STA"){

    } else {
        std::cerr << "server: TCP: wrong syntax\n";
    }

    
    close(fd);
}


void tcp_server(SCORELIST* list, std::string GSPort) {
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
            handleTCP(newfd, list);
            exit(EXIT_SUCCESS);
        }
    }
    freeaddrinfo(res);
    close(fd);
}