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

using namespace std;

#define DEFAULT_IP "localhost"
#define DEFAULT_PORT "58018"

int main(int argc, char **argv){
    int fd, errcode;
    string GSIP, GSPort;
    string toString = "";
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char err_buff[128], buffer[128];

    if(argc == 1) {
        cout << "1\n";
        GSIP = DEFAULT_IP;
        GSPort = DEFAULT_PORT;
    } else {
        cout << "2\n";
        string argv1;
        string argv2;

        switch (argc) {
            case 2:
                argv1 = toString + argv[1];
                if(argv1 == "-n" || argv1 == "-p") {
                    cerr << "player: error: missing argument after '" << argv1 << "' \n";
                } else {
                    cerr << "player: unrecognized command-line option " << argv1 << "\n";
                }
                break;
            
            case 3:
                argv1 = toString + argv[1];
                argv2 = toString + argv[2];
                if(argv1 == "-n"){
                    GSIP = argv2;
                } else if (argv1 == "-p"){
                    GSPort = argv2;
                } else {
                    cerr << "player: unrecognized command-line option " << argv1 << "\n";
                }
                break;
                
            
            default:
                break;
            }

        if(argc == 1){
            if(argv1 == "-n" || argv1 == "")
            
        } else if (argc )
        

        if(argv1 == "-n"){
            GSIP = argv2;
        } else if (argv1 == "-p"){
            GSPort = argv2;
        } else {
            snprintf(err_buff, 128,"player: unrecognized command-line option '%s'/n", argv[1]);
            perror(err_buff);
        }
    } else if (argc == 5){
        cout << "3\n";
        string argv1 = toString + argv[1];
        string argv2 = toString + argv[2];
        if(argv1 == "-n"){
            GSIP = argv2;
        } else if (argv1 == "-p"){
            GSPort = argv2;
        } else {
            snprintf(err_buff, 128,"player: unrecognized command-line option '%s'/n", argv[1]);
            perror(err_buff);
        }
    }

    //cout << GSIP;
    // while (1) {
        

    //     fd = socket(AF_INET,SOCK_STREAM,0); //TCP socket
    //     if (fd == -1) exit(1); //error

    //     memset(&hints,0,sizeof hints);
    //     hints.ai_family = AF_INET; //IPv4
    //     hints.ai_socktype = SOCK_STREAM; //TCP socket
    //     errcode = getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res);
    //     if(errcode!=0)/*error*/exit(1);
    //     n = connect(fd,res->ai_addr,res->ai_addrlen);
    //     if(n==-1)/*error*/exit(1);
    //     n = write(fd,"Hello!\n",7);
    //     if(n==-1)/*error*/exit(1);
    //     n = read(fd,buffer,128);
    //     if(n==-1)/*error*/exit(1);
    //     write(1,"echo: ",6); write(1,buffer,n);

    //     freeaddrinfo(res);
    //     close(fd);
    // }
    
    
}