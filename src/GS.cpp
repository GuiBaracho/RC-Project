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

#define DEFAULT_PORT "58018"
#include "udp_GS.h"
#include "tcp_GS.h"

std::string toString = "";

void readFile(std::string name_file, std::string word_file) {
    std::ifstream file;
    std::string data;
    int count = 2;
    int c = 0;
    file.open(name_file);
    
    if(file.is_open()) {
        //file.seekg(0, file.end); //changing offset to the end 
        //int flength = file.tellg(); //getting the length through offset
        //file.seekg(0, file.beg);
        while (file >> data) {
           if (count % 2 == 0) {
            word_file = word_file + data + " ";
            c++;
           }
           count++;
        }
        //word_file = new char [flength];
        //file.read(word_file, flength);
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


void processInput2GS(char **argv, std::string word_file) {
    std::string argv1 = toString + argv[1];
    readFile(argv1, word_file);
}

void processInput3GS(char **argv, std::string word_file, int &v_mode) {
    std::string argv1 = toString + argv[1];
    std::string argv2 = toString + argv[2];
    readFile(argv1, word_file);

    if(argv2 == "-p") {
        std::cerr << "player: error: missing argument after '" << argv2 << "'" << std::endl;
    } else if (argv2 == "-v") {
        v_mode = 1;
    } else{
        std::cerr << "player: unrecognized command-line option " << argv2 << std::endl;
    }
    exit(EXIT_FAILURE);
}

void processInput4GS(char **argv, std::string word_file, std::string &GSPort) {
    std::string argv1 = toString + argv[1];
    readFile(argv1, word_file);
    std::string argv2 = toString + argv[2];
    std::string argv3 = toString + argv[3];

    if(argv2 == "-p") {
        GSPort = argv3;
    } else if (argv2 == "-v") {
        std::cerr << "player: unrecognized command-line option " << argv3 << "\n";
        exit(EXIT_FAILURE);
    } else {
        std::cerr << "player: unrecognized command-line option " << argv2 << "\n";
        exit(EXIT_FAILURE);
    }
}

void processInput5GS(char **argv, std::string word_file, int &v_mode, std::string &GSPort) {
    std::string argv1 = toString + argv[1];
    readFile(argv1, word_file);
    std::string argv2 = toString + argv[2];
    std::string argv3 = toString + argv[3];
    std::string argv4 = toString + argv[4];

    if(argv2 == "-p") {
        GSPort = argv3;
        if (argv4 == "-v") {
            v_mode = 1;
        } else {
            std::cerr << "player: unrecognized command-line option " << argv4 << "\n";
            exit(EXIT_FAILURE);
        }
    } else if (argv2 == "-v") {
        v_mode = 1;
        if (argv3 == "-p") {
            GSPort = argv4;
        } else {
            std::cerr << "player: unrecognized command-line option " << argv3 << "\n";
            exit(EXIT_FAILURE);
        }
    } else {
        std::cerr << "player: unrecognized command-line option " << argv2 << "\n";
        exit(EXIT_FAILURE);
    }
}

int main (int argc, char **argv) {

    std::string GSPort = DEFAULT_PORT;
    std::string argv1, argv2, argv3, argv4, buffer;
    std::string word, PLID;
    
    
    std::string word_file;
    int v_mode = 0;
    
    switch (argc) {
        case 1:
            std::cerr << "server: error: too few arguments" << std::endl;

        case 2:
            processInput2GS(argv, word_file);
            break;

        case 3:
            processInput3GS(argv, word_file, v_mode);
            break;

        case 4:
            processInput4GS(argv, word_file, GSPort);
            break;

        case 5:
            processInput5GS(argv, word_file, v_mode, GSPort);
            break;

        default:
            std::cerr << "server: error: too many arguments" << std::endl;
            exit(EXIT_FAILURE);
    }

    if (mkdir("ONGOING_GAMES", 0777) == -1) {
        //Directory Already Created
    }
    if (mkdir("FINISHED_GAMES", 0777) == -1) {
        //Directory Already Created
    }

    std::cout << GSPort << std::endl;

    if (fork() == 0) {
        tcp_server(GSPort);
    } else {
        udp_server(word_file, GSPort, v_mode);
    }
    
}

