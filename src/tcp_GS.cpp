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

int v_modetcp;

void create_stateFIN(std::string PLID, std::string name) {
    std::ifstream file;
    std::ofstream newfile;
    std::string code, word, hint, end;
    std::string buffer = "";
    int cont;
    int n = 0;
    std::string path = "./GAMES/" + PLID + "/" + name;
    std::string spath = "./GAMES/STATE_" + PLID + ".txt";
    file.open(path);
    newfile.open(spath, std::ios::trunc);
    newfile.close();

    if(file.is_open()) { 
        newfile.open(spath, std::ios::app);
        std::string data;
        file >> word;
        file >> hint;
        int size = word.length();
        while (file >> data) {
            n = 0;
            cont++;
            if (cont % 2 == 0) {
                if (code == "T") {
                    for (int i = 0; i < size; i++) {
                        if (tolower(word[i]) == tolower(data[0])) {
                            n = 1;
                            buffer = buffer + "     Letter trial: " + data + " - TRUE\n";
                            break;
                        }
                    }
                    if (n == 0) {
                        buffer = buffer + "     Letter trial: " + data + " - FALSE\n";
                    }
                }
                if (code == "G") {
                    buffer = buffer + "     Word guess: " + data + "\n";
                }
            }
            code = data;
        }
        int ntrial = cont/2;
        if(newfile.is_open()) {
            std::string line = "     Last finalized game for player " + PLID + "\n";
            newfile << line;
            line = "     Word: " + word + "; Hint file: " + hint + "\n";
            newfile << line;
            line = "     --- Transactions found: " + std::to_string(ntrial) + " ---\n";
            newfile << line;
            newfile << buffer;
            if (name[16] == 'W') {
                line = "     Termination: WIN\n";
            } else if (name[16] == 'Q') {
                line = "     Termination: QUIT\n";
            } else if (name[16] == 'F') {
                line = "     Termination: FAIL\n";
            }
            newfile << line;
        }
        newfile.close();
        file.close();
    } else {
        //It should exist since I searched in the directory before this function is called
    }
}

int lastGame(int fd, std::string PLID) {
    DIR *dir;
    struct dirent *ent;
    std::string d = "./GAMES/" + PLID;
    int maxi = 0, maxid = 0;
    std::string highname = "";
    std::string toString = "";
    if ((dir = opendir (d.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::string name = ent->d_name + toString;
            if (name == "." || name == "..") {
                continue;
            }
            std::string d = name.substr(0,8);
            int v = std::stoi(d);
            if (v > maxid) {
                highname = name;
                maxid = v;
            }
            else if (v == maxid) {
                std::string t = name.substr(9,6);
                int v = std::stoi(t);
                if (v > maxi) {
                    maxi = v;
                    highname = name;
                    continue;
                } else {
                    continue; //Same numbers;
                }
            }
        }
        if (highname == "") {
            return -1;
        }
        create_stateFIN(PLID, highname);
        closedir (dir);
        return 1;
    } else {
        return -1;
        /* could not open directory */
    }
}

int create_state(int fd, std::string PLID) {
    std::ifstream file;
    std::ofstream newfile;
    std::string code, word;
    std::string solved = "";
    std::string buffer = "";
    int cont;
    int n = 0, c = 0;
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    std::string spath = "./GAMES/STATE_" + PLID + ".txt";
    file.open(path);
    newfile.open(spath, std::ios::trunc);
    newfile.close();

    if(file.is_open()) { 
        newfile.open(spath, std::ios::app);
        std::string data;
        file >> word;
        file >> data;
        int size = word.length();
        for (int j = 0; j < size; j++) {
            solved = solved + "-";
        }
        solved = solved + "\n";
        while (file >> data) {
            n = 0;
            cont++;
            if (cont % 2 == 0) {
                if (code == "T") {
                    c = 0;
                    for (int i = 0; i < size; i++) {
                        if (tolower(word[i]) == tolower(data[0])) {
                            c++;
                            n = 1;
                            if (c == 1) {
                                buffer = buffer + "     Letter trial: " + data + " - TRUE\n";
                            }
                            solved[i] = data[0];
                        }
                    }
                    if (n == 0) {
                        buffer = buffer + "     Letter trial: " + data + " - FALSE\n";
                    }
                }
                if (code == "G") {
                    buffer = buffer + "     Word guess: " + data + "\n";
                }
            }
            code = data;
        }
        int ntrial = cont/2;
        if(newfile.is_open()) {
            std::string line = "     Active game found for player " + PLID + "\n";
            newfile << line;
            line = "     --- Transactions found: " + std::to_string(ntrial) + " ---\n";
            newfile << line;
            newfile << buffer;
            line = "     Solved so far: " + solved;
            newfile << line;
        }
        newfile.close();
        file.close();
        return 0;
    } else {
        return lastGame(fd, PLID);
    }
}

std::string addSpaces(int n){
    std::string space;
    for(int i = 0; i < n; i++){
        space += " ";
    }
    return space;
}

std::string place(int n){
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
        if (v_modetcp == 1) {
            std::cout << "PLID = " << PLID << ": hint: there isn't an ongoing game\n";
        }
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
    if (v_modetcp == 1) {
        std::cout << "PLID = " << PLID << ": hint: send hint file '" << IMGname << "'\n";
    }
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
        if (v_modetcp == 1) {
            std::cout << "scoreboard: scoreboard is empty\n";
        }
        return -1;
    } else {
        msg = "RSB OK TOPSCORES.txt " + std::to_string(80*n_scores + 1) + " " + TITLE+ "\n" + SB_HEADER + "\n";
        write_TCP(fd, msg);
        if (v_modetcp == 1) {
            std::cout << "scoreboard: send scoreboard file TOPSCORES.txt\n";
        }
        for(int i = 0; i < n_scores; i++){
            msg = place(i+1);
            msg += " - ";
            msg += place(list->score[i]);
            msg += "  ";
            msg += list->PLID[i];
            msg += "  ";
            msg += list->word[i];
            msg += addSpaces(40 - list->word[i].length());
            msg += place(list->n_succ[i]);
            msg += "             ";
            msg += place(list->n_succ[i]) + "\n";
            write_TCP(fd, msg);
        }
    }
    return 0;
}

void server_state(int fd, std::string PLID) {
    int n, flength, offset = 0;
    std::string send;
    char buffer[128];

    n = create_state(fd, PLID);

    if (n == -1) {
        if (v_modetcp == 1) {
            std::cout << "PLID = " << PLID << ": state: there isn't an ongoing game\n";
        }
        send = "RST NOK\n";
        n = write(fd,send.c_str(),send.length());
        if(n == -1){
            std::cerr << "server: TCP: write error\n";
            exit(EXIT_FAILURE);
        }
        return;
    }
    std::string fname = "STATE_" + PLID + ".txt";
    std::string name = "./GAMES/STATE_" + PLID + ".txt";
    std::ifstream sfile(name);
    if(sfile.is_open()) {
        sfile.seekg(0, sfile.end); //changing offset to the end 
        flength = sfile.tellg();
    }
    sfile.close();
    if (n == 0) {
        send = "RST ACT " + fname + " " + std::to_string(flength) + " ";
    } else if (n == 1) {
        send = "RST FIN " + fname + " " + std::to_string(flength) + " ";
    }
    n = write(fd, send.c_str(), send.length());
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }

    int size = 128;
    std::ifstream msg(name);
    if(msg.is_open()){
        while(flength > 0){
            msg.seekg(offset, msg.beg);
            msg.read(buffer, size);
            n = write(fd, buffer, size);
            if(n == -1){
                std::cerr << "server: TCP: write error\n";
                exit(EXIT_FAILURE);
            }
            offset += n;
            flength -= n;
            if(flength < size) {
                size = flength;
            }
        }
    }
    msg.close();
    remove(name.c_str());
    n = write(fd,"\n",1);
    if (v_modetcp == 1) {
        std::cout << "PLID = " << PLID << ": state: send state file '" << fname << "'\n";
    }
    if(n == -1){
        std::cerr << "server: TCP: write error\n";
        exit(EXIT_FAILURE);
    }
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
        read_TCP(fd, arg, 7);
        server_state(fd, arg);
    } else {
        std::cerr << "server: TCP: wrong syntax\n";
    }

    close(fd);
}

void tcp_server(SCORELIST* list, std::string GSPort, int v) {
    int fd, newfd, err, n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    std::string b;
    v_modetcp = v;

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