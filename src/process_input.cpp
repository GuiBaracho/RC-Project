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
#include "process_input.h"

std::string toString = "";

void processInput2(char **argv){
    std::string argv1 = toString + argv[1];

    if(argv1 == "-n" || argv1 == "-p") {
        std::cerr << "player: error: missing argument after '" << argv1 << "' \n";
    } else {
        std::cerr << "player: unrecognized command-line option " << argv1 << "\n";
    }
    exit(EXIT_FAILURE);
}

void processInput3(std::string &GSIP, std::string &GSPort, char **argv) {
    std::string argv1 = toString + argv[1];
    std::string argv2 = toString + argv[2];

    if(argv1 == "-n"){
        GSIP = argv2;
    } else if (argv1 == "-p"){
        GSPort = argv2;
    } else {
        std::cerr << "player: unrecognized command-line option " << argv1 << "\n";
        exit(EXIT_FAILURE);
    }
}

void processInput4(char **argv) {
    std::string argv1 = toString + argv[1];
    std::string argv2 = toString + argv[2];
    std::string argv3 = toString + argv[3];

    if(argv1 == "-n" || argv1 == "-p") {
        if(argv1 == argv3){
            std::cerr << "player: error: flag '" << argv3 << "' already used\n";
        } else if (argv3 == "-n" || argv3 == "-p") {
            std::cerr << "player: error: missing argument after '" << argv3 << "' \n";
        } else {
            std::cerr << "player: unrecognized command-line option " << argv3 << "\n";
        }
    } else {
        std::cerr << "player: unrecognized command-line option " << argv1 << "\n";
    }
    exit(EXIT_FAILURE);
}

void processInput5(std::string &GSIP, std::string &GSPort, char **argv) {
    std::string argv1 = toString + argv[1];
    std::string argv2 = toString + argv[2];
    std::string argv3 = toString + argv[3];
    std::string argv4 = toString + argv[4];

    if(argv1 == "-n"){
        GSIP = argv2;
    } else if (argv1 == "-p"){
        GSPort = argv2;
    } else {
        std::cerr << "player: unrecognized command-line option " << argv1 << "\n";
        exit(EXIT_FAILURE);
    }
    if(argv1 == argv3){
        std::cerr << "player: error: flag '" << argv3 << "' already used\n";
        exit(EXIT_FAILURE);
    }
    if(argv3 == "-n"){
        GSIP = argv4;
    } else if (argv3 == "-p"){
        GSPort = argv4;
    } else {
        std::cerr << "player: unrecognized command-line option " << argv2 << "\n";
        exit(EXIT_FAILURE);
    }
}