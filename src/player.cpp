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
//#include <sstream>
#include<bits/stdc++.h>

using namespace std;

#define DEFAULT_IP "localhost"
#define DEFAULT_PORT "58018"


string* readCommand(){
    char c;
    int i = 0;
    string input[2] = {"", ""};

    while ((c = getchar()) == ' ' || c == '\t') {}
    do {
        if (c == ' '|| c == '\t'){
            i++;
        } else {
            input[i] += c;
        }
        if (i > 1){
            break;
        }
    } while ((c = getchar()) == '\n');
    
}



int main(int argc, char **argv){
    int fd, errcode;
    string GSIP = DEFAULT_IP;
    string GSPort = DEFAULT_PORT;
    string toString = "";
    string argv1, argv2, argv3, argv4;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char err_buff[128], buffer[128];

    switch (argc) {
        case 1:
            break;

        case 2:
            argv1 = toString + argv[1];

            if(argv1 == "-n" || argv1 == "-p") {
                cerr << "player: error: missing argument after '" << argv1 << "' \n";
            } else {
                cerr << "player: unrecognized command-line option " << argv1 << "\n";
            }
            exit(0);
            
        case 3:
            argv1 = toString + argv[1];
            argv2 = toString + argv[2];

            if(argv1 == "-n"){
                GSIP = argv2;
            } else if (argv1 == "-p"){
                GSPort = argv2;
            } else {
                cerr << "player: unrecognized command-line option " << argv1 << "\n";
                exit(0);
            }
            break;
            
        case 4:
            argv1 = toString + argv[1];
            argv2 = toString + argv[2];
            argv3 = toString + argv[3];

            if(argv1 == "-n" || argv1 == "-p") {
                if(argv1 == argv3){
                    cerr << "player: error: flag '" << argv3 << "' already used\n";
                } else if (argv3 == "-n" || argv3 == "-p") {
                    cerr << "player: error: missing argument after '" << argv3 << "' \n";
                } else {
                    cerr << "player: unrecognized command-line option " << argv3 << "\n";
                }
            } else {
                cerr << "player: unrecognized command-line option " << argv1 << "\n";
            }
            exit(0);
            break;
            
        case 5:
            argv1 = toString + argv[1];
            argv2 = toString + argv[2];
            argv3 = toString + argv[3];
            argv4 = toString + argv[4];

            if(argv1 == "-n"){
                GSIP = argv2;
            } else if (argv1 == "-p"){
                GSPort = argv2;
            } else {
                cerr << "player: unrecognized command-line option " << argv1 << "\n";
                exit(0);
            }
            if(argv1 == argv3){
                cerr << "player: error: flag '" << argv3 << "' already used\n";
                exit(0);
            }
            if(argv3 == "-n"){
                GSIP = argv4;
            } else if (argv3 == "-p"){
                GSPort = argv4;
            } else {
                cerr << "player: unrecognized command-line option " << argv2 << "\n";
                exit(0);
            }

        default:
            cerr << "player: error: too many arguments\n";
            exit(0);
    }

    cout << GSIP << "\n";
    cout << GSPort << "\n";

    char a = 'a';

    string teste = toString + a;

    cout << teste << endl;
    
    return 0;
}