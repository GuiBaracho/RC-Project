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

#define DEFAULT_PORT "58018"

std::string toString = "";
int v_mode;

void readFile(std::string name_file, char *&word_file) {
    std::string data;
    std::ifstream file;
    file.open(name_file);
    if(file.is_open()) {
        file.seekg(0, file.end); //changing offset to the end 
        int flength = file.tellg(); //getting the length through offset
        file.seekg(0, file.beg);

        word_file = new char [flength];
        file.read(word_file, flength);
        std::cout << word_file << std::endl;
    }
    else {
        std::cout << "Following file " << name_file << "doesn't exist" << std::endl;
    }
    file.close();
}

void processInput2GS(char **argv, char *&word_file) {
    std::string argv1 = toString + argv[1];
    readFile(argv1, word_file);
}

void processInput3GS(char **argv) {
    std::string argv2 = toString + argv[2];

    if(argv2 == "-p") {
        std::cerr << "player: error: missing argument after '" << argv2 << "'" << std::endl;
    } else if (argv2 == "-v") {
        v_mode = 1;
    } else{
        std::cerr << "player: unrecognized command-line option " << argv2 << std::endl;
    }
    exit(EXIT_FAILURE);
}

void processInput4GS(char **argv, char *&word_file, std::string &GSPort) {
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

void processInput5GS(char **argv, char *&word_file, std::string &GSPort) {
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

    int fd, trial;
    std::string GSPort = DEFAULT_PORT;
    std::string argv1, argv2, argv3, argv4, buffer;
    std::string word, PLID;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char *word_file;
    v_mode = 0;

    switch (argc) {
        case 1:
            std::cerr << "server: error: too few arguments" << std::endl;

        case 2:
            processInput2GS(argv, word_file);
            break;

        case 3:
            processInput3GS(argv);

        case 4:
            processInput4GS(argv, word_file, GSPort);

        case 5:
            processInput5GS(argv, word_file, GSPort);

        default:
            std::cerr << "server: error: too many arguments" << std::endl;
            exit(EXIT_FAILURE);
    }
    
}
