#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

#include "connections.h"

int value(std::string command){
    std::stringstream ss;
    int val;
    ss << command;
    ss >> val;
    return val;
}

int calculate_maxerrors(std::string word) {
    int max_errors;
    int word_size = word.length();
    if (word_size <= 6) {
        max_errors = 7;
    } else if (word_size >= 7 && word_size <= 10) {
        max_errors = 8;
    } else if (word_size > 10) {
        max_errors = 9;
    }
    return max_errors;
}

std::string toStringT(int dt) {
    std::string t;
    if (dt < 10) {
        t = std::to_string(0) + std::to_string(dt);
    } else {
        t = std::to_string(dt);
    }
    return t;
}

std::string getTimeInString(std::string state) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int year = 1900 + ltm->tm_year;
    int month = 1 + ltm->tm_mon;
    int day = ltm->tm_mday;
    int hour = ltm->tm_hour;
    int minute = ltm->tm_min;
    int second = ltm->tm_sec-1;
    std::string date = std::to_string(year) + toStringT(month) + toStringT(day);
    std::string time = toStringT(hour) + toStringT(minute) + toStringT(second);
    std::string dt = date + "_" + time + "_" + state;
    return dt;
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

int createFile(std::string PLID, std::string &wordhint) {
    std::ifstream file;
    std::string gword;
    std::stringstream ss(wordhint);
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    int cont = 0;

    file.open(path);
    if(file.is_open()) {
        std::string data;
        while (file >> data) {
            cont++;
            if (cont == 1) {
                gword = data;
            }
        }
        if (cont == 2) {
            wordhint = gword;
            file.close();
            return 0;
        } 
        ss >> gword;
        wordhint = gword;
        file.close();
        return 1;
    }
    else {
        std::ofstream new_file;
        std::string name = "./GAMES/GAME_" + PLID + ".txt";
        new_file.open(name);
        if (new_file.is_open()) {
            new_file << wordhint;
        } else {
            std::cout << "why is not opened\n";
        }
        ss >> gword;
        wordhint = gword;
        new_file.close();
        return 0;
    }
    return -1;
}

void moveToDone(std::string type, std::string PLID) {
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    std::string dir = "./GAMES/" + PLID;
    if (mkdir(dir.c_str(), 0777) == -1) {
        //Directory Already Created
    }
    std::string new_name = getTimeInString(type);
    std::string new_path = "./GAMES/" + PLID + "/" + new_name + ".txt";
    rename(path.c_str(), new_path.c_str());
}

void writeFile(std::string type, std::string PLID, std::string guess) {
    std::string msg = type + " " + guess + "\n";
    std::ofstream file;
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    file.open(path, std::ios::app);

    if(file.is_open()) {
        std::string data;
        file << msg;
    }

    file.close();
}

int searchOnFile(std::string type, std::string PLID, std::string trial, std::string &word, std::string letter, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    int err, ntrial, max_errors, size;
    int cont = 0;
    int dupe = 0;
    int errors = 0;
    int match = 0;
    std::ifstream file;
    ntrial = value(trial);
    std::string msg, code;
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    file.open(path);

    if(file.is_open()) {
        std::string data;
        file >> word;
        file >> data;
        size = word.length();
        while (file >> data) {
            std::cout << "the data is: " << data << std::endl;
            cont++;
            if (cont % 2 == 0) {
                if (strcasecmp(letter.c_str(), data.c_str()) == 0) {
                    dupe = 1;
                }
                if (code == "T") {
                    for (int i = 0; i < size; i++) {
                        if (tolower(word[i]) == tolower(data[0])) {
                            errors --;
                            match++;
                            break;
                        }
                    }
                    errors++;
                } else if (code == "G") {
                    errors++;
                }
            }
            code = data;
        }
        if (type == "RLG") {
            for (int i = 0; i < size; i++) {
                if (tolower(word[i]) == tolower(letter[0])) {
                    std::cout << "errors2: " << errors << std::endl;
                    errors--;
                    break;
                }
            }
            errors++;
        } else {
            if (strcasecmp(word.c_str(), letter.c_str()) != 0) {
                errors++; }
        }
        
        max_errors = calculate_maxerrors(word);
        std::cout << "the max is " << max_errors << "but we have: " << errors << std::endl;
        ntrial = cont/2 + 1;
        if (max_errors < errors) {
            msg = type + " OVR " + std::to_string(ntrial) + "\n";
            err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
            file.close();
            if (type == "RLG")  {
                writeFile("T", PLID, letter);
            } else {
                writeFile("G", PLID, letter);
            }
            moveToDone("F", PLID);
            return 1;
        }
        if (dupe == 1) {
            msg = type + " DUP " + std::to_string(ntrial-1) + "\n";
            err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
            file.close();
            return 1;
        } 
        else if (ntrial != value(trial)) {
            msg = type + " INV " + std::to_string(ntrial-1) + "\n";
            err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
            file.close();
            return 1;
        }
        file.close();
        return 0;
    }
    else { 
        std::string msg = type + " ERR\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        return 1;
    }
}

void server_start(std::string word_file, std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {

    std::string word, gword, hint;
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
            std::cout << gword << std::endl;
            ss >> word;
            hint = word;
            break;
        }
        ss >> word;
        count++;
    }
   
    std::cout << count << " " << gword << " " << hint << std::endl;
    word = gword + " " + hint + "\n";
    n = createFile(PLID, word);
    std::cout << "The word is " << word << std::endl;
    if (n == 1) {
        std::string msg = "RSG NOK\n";
        
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) std::cout << "Deu erro\n";
        std::cout << msg << std::endl;

    } else if (n == 0) {
        int word_size = word.length();
        max_errors = calculate_maxerrors(word);

        std::string msg = "RSG OK " + std::to_string(word_size) + " " + std::to_string(max_errors) + "\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) std::cout << "Deu erro\n";

        std::cout << msg << "\n";
    }
}

void server_play(std::string PLID, std::string trial, std::string letter, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    std::string word;
    std::string msg = "";
    int err;
    int ok = 0;
    if (isdigit(letter[0]) == true) {
        std::string msg = "RLG ERR\n";
        std::cout << msg << std::endl;
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        return;
    }
    err = searchOnFile("RLG", PLID, trial, word, letter, fd, addr, addrlen);
    if (err == 1) {
        return;
    }
    int size = word.length();
    for (int i = 0; i < size ; i++) {
        if (tolower(word[i]) == tolower(letter[0])) {
            msg = msg + " " + std::to_string(i+1);
            ok++;
        }
    }
    if (ok > 0) {
        std::string add = "RLG OK " + trial + " " + std::to_string(ok) + msg + "\n";
        std::cout << add << std::endl;
        err = sendto(fd,add.c_str(),add.length(),0,(struct sockaddr*) &addr, addrlen);
        writeFile("T", PLID, letter);
    } else {
        msg = "RLG NOK " + trial + "\n";
        std::cout << msg << std::endl;
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        writeFile("T", PLID, letter);
    }
    
}

void server_guess(std::string PLID, std::string trial, std::string gword, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) { 
    std::string word;
    std::string msg;
    int err;
    int ok = 0;
    int size = gword.length();

    for (int i = 0; i < size; i++) {
        if (isdigit(gword[i]) == true) {
            msg = "RWG ERR\n";
            std::cout << msg << std::endl;
            err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
            return;
        }
    }

    err = searchOnFile("RWG", PLID, trial, word, gword, fd, addr, addrlen);
    if (err == 1) {
        return;
    }
    if (strcasecmp(word.c_str(), gword.c_str()) == 0) {
        msg = "RWG WIN " + trial + "\n";
        std::cout << msg << std::endl;
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        writeFile("G", PLID, gword);
        moveToDone("W", PLID);
    } else {
        msg = "RWG NOK " + trial + "\n";
        std::cout << msg << std::endl;
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        writeFile("G", PLID, gword);
    }
}

void server_quit(std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    int err;
    std::ifstream file;
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    file.open(path);
    if(file.is_open()) {
        file.close();
        moveToDone("Q", PLID);
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
    int fd, err, n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    std::string toString = "";
    std::string b;
    
    connectUDPServer(GSPort, fd, hints, res);

    while(1) {
        n = 0;
        b.clear();
        addrlen = sizeof(addr);
        err = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if(err == -1) {
            std::cout << "Deu erro1\n" << strerror(errno);
        }
        b.append(buffer, 0, err);
        std::cout << b << "\n";
        std::stringstream ss(b);
        std::string m, type, PLID, letter, word, trial;
        int size;
        int counter = 0;
        
        while(ss >> m) {
            std::cout << m << std::endl;
            counter++;
            if (counter == 1) {
                type = m;
                if (!(type == "SNG" || type == "PLG" || type == "PWG" || type == "QUT")) {
                    std::cout << type << "is wrong?" << std::endl;
                    n = 1;
                    break;
                }
            } else if (counter == 2) {
                PLID = m;
                size = PLID.length();
                for (int i = 0; i <size; i++) {
                    if(isdigit(PLID[i]) == false) {
                        n = 1;
                        break;
                    }
                }
                if (size != 6) {
                    n = 1;
                    break;
                }
            } else if (counter >= 3) {
                if (type == "PLG" || type == "PWG") {
                    if (counter == 3 && type == "PLG") {
                        letter = m;
                        std::cout << "the letter is" << letter << std::endl;
                    } else if (counter == 3 && type == "PWG") {
                        word = m;
                    } else if (counter == 4) {
                        trial = m;
                        break;
                    }
                }
                else {
                    std::cout << "if it's here then fuck you" << std::endl;
                    n = 1;
                    break;
                }
            }
        } 
        std::cout << n << " and counter is " << counter << " and type is " << type << std::endl;
        if (n == 0) {
            if (type == "SNG" && counter == 2) {
                server_start(word_file, PLID, fd, addr, addrlen);
            } else if (type == "PLG" && counter == 4) {
                server_play(PLID, trial, letter, fd, addr, addrlen);
            } else if (type == "PWG" && counter == 4) {
                server_guess(PLID, trial, word, fd, addr, addrlen);
            } else if (type == "QUT" && counter == 2) {
                server_quit(PLID, fd, addr, addrlen);
            } else {
                error_message(type, PLID, v_mode, fd, addr, addrlen);
            }
        } else {
            error_message(type, PLID, v_mode, fd, addr, addrlen);
        }
    }

    freeaddrinfo(res);
    close(fd);
}