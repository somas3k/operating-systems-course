#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <signal.h>

#include <netinet/in.h>
#include <sys/un.h>

#include <arpa/inet.h>

#include "common.h"

void showUsage();
void quitter(int signo);
void cleanup(void);
char* validateName(char* name);
int validateType(char* type);
char* validatePath(char* path);
int validateAddress(char* ip);
unsigned short validatePort(unsigned short port);
int connectLocal(char* path);
int connectWeb(int address, short port);
void handleReq();
void sendName(char type);
void registerClient(int sfd);

int sfd; // socket file descriptor
char* name;

int main(int argc, char** argv){
    if((argc != 4 ) && (argc != 5)) showUsage();
    name = validateName(argv[1]);
    int type = validateType(argv[2]);

    if(signal(SIGINT, quitter) == SIG_ERR) print_error("Signal failed!");
    if(signal(SIGTERM, quitter) == SIG_ERR) print_error("Signal failed!");
    if(atexit(cleanup) != 0) print_error("At exit failed!"); // procedura sprzatajaca

    if((type == LOCAL) || (argc == 4)){
        char* path = validatePath(argv[3]);
        sfd = connectLocal(path);
    }else if((type == WEB) || (argc == 5)){
        int address = validateAddress(argv[3]);
        unsigned short port = validatePort((unsigned short)atoi(argv[4]));
        sfd = connectWeb(address, port);
    }else showUsage();

    registerClient(sfd);

    while(1){
        char mType;
        if(read(sfd, &mType, 1) != 1) print_error("CLIENT: reading type failed!");

        if(mType == PING){
            sendName(PONG);
        }else if(mType == REQ){
            handleReq();
        }
    }
}

void sendName(char type){
    char resType = type;
    short mLen = (short)(strlen(name) + 1);
    short mLen1 = htons((uint16_t)mLen);
    if(write(sfd,&resType, 1) != 1) print_error("CLIENT: resType failed!");
    if(write(sfd, &mLen1, 2) != 2) print_error("CLIENT:  writing mLen failed!");
    if(write(sfd, name, (size_t)mLen) != mLen) print_error("CLIENT: writing name failed!");
}

void handleReq(){
    char command;
    int ctn, op1, op2, result;

    if(read(sfd, &command, 1) != 1) print_error("CLIENT: reading command failed!");
    if(read(sfd, &ctn, sizeof(int)) != sizeof(int)) print_error("CLIENT: reading ctn failed!");
    if(read(sfd, &op1, sizeof(int)) != sizeof(int)) print_error("CLIENT: reading op1 failed!");
    if(read(sfd, &op2, sizeof(int)) != sizeof(int)) print_error("CLIENT: reading op2 failed!");

    op1 = ntohl((uint32_t)op1);
    op2 = ntohl((uint32_t)op2);

    switch(command){
        case '+':
            result = op1 + op2;
            break;
        case '-':
            result = op1 - op2;
            break;
        case '/':
            result = op1 / op2;
            break;
        case '*':
            result = op1 * op2;
            break;
        default:
            printf("CLIENT: unknown command!");
            result = 0;
            break;
    }

    short mLen = (short)(strlen(name) + 1);
    short mLen1 = htons((uint16_t)mLen);
    result = htonl((uint32_t)result);
    char resType = RESULT;

    if(write(sfd,&resType, 1) != 1) print_error("CLIENT: RESULT failed!");
    if(write(sfd, &mLen1, 2) != 2) print_error("CLIENT: writing result mLen failed!");
    if(write(sfd, &ctn, sizeof(int)) != sizeof(int)) print_error("CLIENT: writing ctn failed!");
    if(write(sfd, &result, sizeof(int)) != sizeof(int)) print_error("CLIENT: writing result failed!");
    if(write(sfd, name, (size_t)mLen) != mLen) print_error("CLIENT: writing result name failed!");
}

int connectLocal(char* path){
    struct sockaddr_un localAddress;
    localAddress.sun_family = AF_UNIX;
    for(int i=0; i<strlen(path) + 1; i++){
        localAddress.sun_path[i] = path[i];
    }

    int localSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(localSocket == -1) print_error("CLIENT: creating localSocket failed!");

    if(connect(localSocket, &localAddress, sizeof(localAddress)) == -1) print_error("CLIENT: localSocket connecting failed!");

    return localSocket;
}
int connectWeb(int address, short port){
    struct sockaddr_in webAddress;
    webAddress.sin_family = AF_INET;
    webAddress.sin_port = htons((uint16_t)port);
    webAddress.sin_addr.s_addr = inet_addr("83.242.74.12");

    int webSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(webSocket == -1) print_error("Creating webSocket failed!");

    if(connect(webSocket, &webAddress, sizeof(webAddress)) == -1) print_error("CLIENT: webSocket connecting failed!");

    return webSocket;
}

void registerClient(int sfd){
    char mType;
    sendName(LOGIN);
    if(read(sfd, &mType, 1) != 1) print_error("Receiving login response failed!");

    if(mType == FAILSIZE){
        printf("Too many clients, couldnt log in!\n");
        exit(2);
    }else if(mType == FAILNAME){
        printf("Name already in use, couldnt log in!\n");
        exit(2);
    }else if(mType == SUCCESS){
        printf("Logged in successfully!\n");
    }else{
        print_error("Unpredicted behaviour in registerClient!");
    }

}

void showUsage(){
    printf("Use program like <name> <type> <address or path> (<port>)\n");
    exit(1);
}

char* validateName(char* name){
    int l = (int)strlen(name);
    if((l < 2) || (l > 20)){
        print_error("Name must be of length between 2 and 20");
    }
    return name;
}
int validateType(char* type){
    if(strcmp(type, "loc") == 0){
        return LOCAL;
    }else if(strcmp(type, "web") == 0){
        return WEB;
    }
    else{
        print_error("Type must be loc or web!");
        return -1;
    }
}
char* validatePath(char* path){
    int l = (int)strlen(path);
    if((l < 1) || (l > UNIX_PATH_MAX)){
        printf("Path must be of length between 1 and %d\n", UNIX_PATH_MAX);
        exit(1);
    }
    return path;
}
int validateAddress(char* ip){
    int x = inet_addr(ip);
    if(x == -1) print_error("CLIENT: inet_addr failed! Adrees is wrong");
    return x;
}
unsigned short validatePort(unsigned short port){
    if((port < 1024) || (port > 60999)){
        print_error("Port must be a number between 1024 and 60999!");
    }
    return port;
}

void quitter(int signo){
    sendName(LOGOUT);
    exit(2);
}

void cleanup(void){
    int allOk = 1;

    if(shutdown(sfd, SHUT_RDWR) == -1){
        printf("Error shutdowning client socket! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if(close(sfd) == -1){
        printf("Error closing client socket! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }

    if(allOk == 1) printf("All closed down!\n");
}
