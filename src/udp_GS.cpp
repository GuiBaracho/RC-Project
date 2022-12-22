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
#include <cmath>
#include <dirent.h> 

#include "connections.h"
#include "udp_GS.h"

int v_mode;

int get_val(std::string command){
    std::stringstream ss;
    int val;
    ss << command;
    ss >> val;
    return val;
}

void create_stateFIN(std::string PLID, std::string name) {
    std::ifstream file;
    std::ofstream newfile;
    std::string code, word, hint, end;
    std::string buffer = "";
    int cont;
    int n = 0;
    std::string path = "./GAMES/" + PLID + "/" + name;
    std::string spath = "STATE_" + PLID + ".txt";
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

void lastGame(std::string PLID) {
    DIR *dir;
    struct dirent *ent;
    std::string d = "./GAMES/" + PLID;
    int maxi = 0, maxid = 0;
    std::string highname;
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
    create_stateFIN(PLID, highname);
    closedir (dir);
    } else {
        /* could not open directory */
    }
}

void create_state(std::string PLID) {
    std::ifstream file;
    std::ofstream newfile;
    std::string code, word;
    std::string solved = "";
    std::string buffer = "";
    int cont;
    int n = 0;
    int c = 0;
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    std::string spath = "STATE_" + PLID + ".txt";
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
    } else {
        lastGame(PLID);
    }
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
    int second = ltm->tm_sec;
    std::string date = std::to_string(year) + toStringT(month) + toStringT(day);
    std::string time = toStringT(hour) + toStringT(minute) + toStringT(second);
    std::string dt = date + "_" + time + "_" + state;
    return dt;
}


void error_message(std::string command, std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    int err;
    std::string msg;
    if (command == "SNG") {
        msg = "RSG ERR\n";
    } else if (command == "PLG") {
        msg = "RLG ERR\n";
    } else if (command == "PWG") {
        msg = "RWG ERR\n";
    } else if (command == "QUT") {
        msg = "RQT ERR\n";
    } else {
        msg = "ERR\n";
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": The request " << command << " doesn't exist\n";
        }
    }
    if (msg != "ERR\n") {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": The request " << command << " has syntax errors\n";
        }
    }
    err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
    if(err == -1) {
        std::cout << "server: UDP: sendto error\n";
        exit(EXIT_FAILURE);
    }
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
            //it will create it if it doesn't exist so this shouldn't happen 
        }
        ss >> gword;
        wordhint = gword;
        new_file.close();
        return 0;
    }
    return -1;
}

std::string createScore(std::string path, std::string &word, std::string &n_succ, std::string &n_trials) {
    int size, ntrials;
    int n = 0;
    int nsucc = 0;
    float score, nt;
    std::ifstream file;
    std::string code, s;
    file.open(path);

    if(file.is_open()) {
        std::string data;
        file >> word;
        file >> data;
        size = word.length();
        while (file >> data) {
            n++;
            if (n % 2 == 0 && code == "T") {
                for (int i = 0; i < size; i++) {
                    if (tolower(word[i]) == tolower(data[0])) {
                        nsucc++;
                        break; }
                }
            }
            if (n % 2 == 0 && code == "G") {
                if (strcasecmp(word.c_str(), data.c_str()) == 0) {
                    nsucc++;
                }
            }
            code = data; 
        }
    }
    file.close();
    nt = n/2;
    ntrials = n/2;
    score = (nsucc/nt);
    int fscore = round(score * 100);
    if (fscore < 10) {
        s = std::to_string(0) + std::to_string(0) + std::to_string(fscore);
    } else if (fscore < 100) {
        s = std::to_string(0) + std::to_string(fscore);
    } else {
        s = std::to_string(fscore);
    }
    n_succ = std::to_string(nsucc);
    n_trials = std::to_string(ntrials);
    return s;
}

void createScoreFile(std::string PLID, std::string path, std::string date) {
    std::string score, word, n_succ, n_trials;
    std::string year = date.substr(0, 4);
    std::string month = date.substr(4, 2);
    std::string day = date.substr(6, 2);
    std::string time = date.substr(9, 6);
    std::string d = day + month + year + "_" + time;

    score = createScore(path, word, n_succ, n_trials);
    std::string s_path = "./SCORES/" + score + "_" + PLID + "_" + d + ".txt";

    std::string msg = score + " " + PLID + " " + word + " " + n_succ + " " + n_trials + "\n";
    std::ofstream file;
    file.open(s_path);
    if(file.is_open()) {
        file << msg;
    }
    file.close();
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
    if (type == "W") {
        createScoreFile(PLID, new_path, new_name);
    }
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
    std::ifstream file;
    ntrial = get_val(trial);
    std::string msg, code;
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    file.open(path);

    if(file.is_open()) {
        std::string data;
        file >> word;
        file >> data;
        size = word.length();
        while (file >> data) {
            cont++;
            if (cont % 2 == 0) {
                if (strcasecmp(letter.c_str(), data.c_str()) == 0) {
                    dupe = 1;
                }
                if (code == "T") {
                    for (int i = 0; i < size; i++) {
                        if (tolower(word[i]) == tolower(data[0])) {
                            errors --;
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
        file.close();
        if (type == "RLG") {
            for (int i = 0; i < size; i++) {
                if (tolower(word[i]) == tolower(letter[0])) {
                    break;
                }
            }
            errors++;
        } else {
            if (strcasecmp(word.c_str(), letter.c_str()) != 0) {
                errors++; }
        }
        max_errors = calculate_maxerrors(word);
        ntrial = cont/2 + 1;
        if (max_errors < errors) {
            if (v_mode == 1 && type == "RLG") {
                std::cout << "PLID = " << PLID << ": play letter: '" << letter << "' - miss (max errors reached; game over)\n";
            } else if (v_mode == 1 && type == "RWG") {
                std::cout << "PLID = " << PLID << ": guess word: '" << letter << "' - miss (max errors reached; game over)\n";
            }
            msg = type + " OVR " + std::to_string(ntrial) + "\n";
            err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
            if(err == -1) {
                std::cout << "server: UDP: sendto error\n";
                exit(EXIT_FAILURE);
            }
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
            if (v_mode == 1 && type == "RLG") {
                std::cout << "PLID = " << PLID << ": play letter: '" << letter << "' - duplicated\n";
            } else if (v_mode == 1 && type == "RWG") {
                std::cout << "PLID = " << PLID << ": guess word: '" << letter << "' - duplicated\n";
            }
            msg = type + " DUP " + std::to_string(ntrial-1) + "\n";
            err = sendto(fd, msg.c_str(), msg.length(), 0, (struct sockaddr*) &addr, addrlen);
            if(err == -1) {
                std::cout << "server: UDP: sendto error\n";
                exit(EXIT_FAILURE);
            }
            return 1;
        } 
        else if (ntrial != get_val(trial)) {
            if (v_mode == 1 && type == "RLG") {
                std::cout << "PLID = " << PLID << ": play letter: '" << word << "' - invalid trial\n";
            } else if (v_mode == 1 && type == "RWG") {
                std::cout << "PLID = " << PLID << ": guess word: '" << word << "' - invalid trial\n";
            }
            msg = type + " INV " + std::to_string(ntrial-1) + "\n";
            err = sendto(fd, msg.c_str(), msg.length(), 0, (struct sockaddr*) &addr, addrlen);
            if(err == -1) {
                std::cout << "server: UDP: sendto error\n";
                exit(EXIT_FAILURE);
            }
            return 1;
        }
        return 0;
    }
    else { 
        if (v_mode == 1 && type == "RLG") {
            std::cout << "PLID = " << PLID << ": play letter: '" << word << "' - ERR (no ongoing game)\n";
        } else if (v_mode == 1 && type == "RWG") {
            std::cout << "PLID = " << PLID << ": guess word: '" << word << "' - ERR (no ongoing game)\n";
        }
        std::string msg = type + " ERR\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        return 1;
    }
}

int win_play(std::string PLID, std::string trial, std::string letter, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {
    int size, err;
    int count = 0;
    int match = 0;
    std::ifstream file;
    std::string msg, code, word;
    std::string path = "./GAMES/GAME_" + PLID + ".txt";
    file.open(path);

    if(file.is_open()) {
        std::string data;
        file >> word;
        file >> data;
        size = word.length();
        while (file >> data) {
            count++;
            if (count % 2 == 0 && code == "T") {
                for (int i = 0; i < size; i++) {
                    if (tolower(word[i]) == tolower(data[0])) {
                         match++; }
                } 
            }
            code = data;
        }
    } else {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": play letter: '" << letter << "' - ERR (no ongoing game)\n";
        }
        std::string msg = "RLG ERR\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        return 1;
    }
    for (int i = 0; i < size; i++) {
        if (tolower(word[i]) == tolower(letter[0])) {
            match++;
        }
    }
    if(match == size) {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": play letter: '" << letter << "' - WIN (game ended)\n";
        }
        msg = "RLG WIN " + trial + "\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_play: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        writeFile("T", PLID, letter);
        moveToDone("W", PLID);
        file.close();
        return 1;
    }
    file.close();
    return 0;
}

void server_start(std::string word_file, std::string PLID, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) {

    std::string word, gword, hint;
    int err, num, nword, max_errors, n;
    int count = 1;
    time_t t;
    srand((unsigned) time(&t));
    std::stringstream ss(word_file);

    ss >> word;
    nword = get_val(word);
    num = (rand() % nword) + 1;
    while(ss >> word) {
        if (count == num) {
            gword = word;
            ss >> word;
            hint = word;
            break;
        }
        ss >> word;
        count++;
    }

    word = gword + " " + hint + "\n";
    n = createFile(PLID, word);
    int word_size = word.length();
    if (n == 1) {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": New Game: There is already one ongoing: Word = '" << word << "' (" << word_size << " letters)\n";
        }
        std::string msg = "RSG NOK\n";
        
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_start: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        std::cout << msg << std::endl;

    } else if (n == 0) {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": New Game: Word = '" << word << "' (" << word_size << " letters)\n";
        }
        max_errors = calculate_maxerrors(word);
        std::string msg = "RSG OK " + std::to_string(word_size) + " " + std::to_string(max_errors) + "\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_start: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }

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
        if(err == -1) {
            std::cout << "server_play: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        return;
    }
    err = searchOnFile("RLG", PLID, trial, word, letter, fd, addr, addrlen);
    if (err == 1) return;
    err = win_play(PLID, trial, letter, fd, addr, addrlen);
    if (err == 1) return;

    int size = word.length();
    for (int i = 0; i < size ; i++) {
        if (tolower(word[i]) == tolower(letter[0])) {
            msg = msg + " " + std::to_string(i+1);
            ok++;
        }
    }
    if (ok > 0) {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": play letter: '" << letter << "' - " << ok << " hits (word not guessed)\n";
        }
        std::string add = "RLG OK " + trial + " " + std::to_string(ok) + msg + "\n";
        std::cout << add << std::endl;
        err = sendto(fd,add.c_str(),add.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_play: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        writeFile("T", PLID, letter);
    } else {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": play letter: '" << letter << "' - miss\n";
        }
        msg = "RLG NOK " + trial + "\n";
        std::cout << msg << std::endl;
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_play: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        writeFile("T", PLID, letter);
    }
}

void server_guess(std::string PLID, std::string trial, std::string gword, int &fd, struct sockaddr_in &addr, socklen_t &addrlen) { 
    std::string word;
    std::string msg;
    int err;
    int ok = 0;
    int size = gword.length();

    //Verifying that the word doesn't have numbers
    for (int i = 0; i < size; i++) {
        if (isdigit(gword[i]) == true) {
            if (v_mode == 1) {
                std::cout << "PLID = " << PLID << ": guess word: '" << gword << "' - ERR (word has numbers)\n";
            }
            msg = "RWG ERR\n";
            std::cout << msg << std::endl;
            err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
            if(err == -1) {
                std::cout << "server_guess: UDP: sendto error\n";
                exit(EXIT_FAILURE);
            }
            return;
        }
    }

    err = searchOnFile("RWG", PLID, trial, word, gword, fd, addr, addrlen);
    if (err == 1) {
        return;
    }
    if (strcasecmp(word.c_str(), gword.c_str()) == 0) {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": guess word: '" << gword << "' - WIN (game ended)\n";
        }
        msg = "RWG WIN " + trial + "\n";
        std::cout << msg << std::endl;
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_guess: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        writeFile("G", PLID, gword);
        moveToDone("W", PLID);
    } else {
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": guess word: '" << gword << "' - MISS\n";
        }
        msg = "RWG NOK " + trial + "\n";
        std::cout << msg << std::endl;
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_guess: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
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
        if (v_mode == 1) {
            std::cout << "PLID = " << PLID << ": quit: there is no ongoing game to quit\n";
        }
        std::string msg = "RQT NOK\n";
        err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
        if(err == -1) {
            std::cout << "server_quit: UDP: sendto error\n";
            exit(EXIT_FAILURE);
        }
        return;
    }
    if (v_mode == 1) {
        std::cout << "PLID = " << PLID << ": quit: ongoing game quit\n";
    }
    std::string msg = "RQT OK\n";
    err = sendto(fd,msg.c_str(),msg.length(),0,(struct sockaddr*) &addr, addrlen);
    if(err == -1) {
        std::cout << "server_quit: UDP: sendto error\n";
        exit(EXIT_FAILURE);
    }
}


void udp_server(std::string word_file, std::string GSPort, int v) {
    int fd, err, n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    std::string b;
    v_mode = v;
    
    connectUDPServer(GSPort, fd, hints, res);

    while(1) {
        n = 0;
        b.clear();
        addrlen = sizeof(addr);
        err = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if(err == -1) {
            std::cout << "server: UDP: recvfrom error\n";
            exit(EXIT_FAILURE);
        }
        b.append(buffer, 0, err);
        std::stringstream ss(b);
        std::string PLID = "";
        std::string m, type, letter, word, trial;
        int size;
        int counter = 0;
        
        while(ss >> m) {
            counter++;
            if (counter == 1) {
                type = m;
                if (!(type == "SNG" || type == "PLG" || type == "PWG" || type == "QUT")) {
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
                        if (m.length() == 1) {
                            letter = m;
                        } else {
                            n = 1;
                            break;
                        }
                    } else if (counter == 3 && type == "PWG") {
                        if (m.length() >= 3) {
                             word = m;
                        } else {
                            n = 1;
                            break;
                        }
                    } else if (counter == 4) {
                        trial = m;
                        break;
                    }
                } else {
                    n = 1;
                    break;
                }
            }
        } 
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
                error_message(type, PLID, fd, addr, addrlen);
            }
        } else {
            error_message(type, PLID, fd, addr, addrlen);
        }
    }

    freeaddrinfo(res);
    close(fd);
}