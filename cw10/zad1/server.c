#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <semaphore.h>

#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#include <netinet/in.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <arpa/inet.h>


#include <netinet/in.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netdb.h>

#include "common.h"

void showUsage();
unsigned short validatePort(unsigned short port);
char* validatePath(char* path);
void quitter(int signo);
void cleanup(void);
int getWebSocket(short portNum);
int getLocalSocket(char* path);
void* pingerJob(void*);
void* commanderJob(void*);
int prepareMonitor();
void handleNewRequest(int socket);
void handleNewMessage(int fd);
void tryRegister(int fd, char* name);
void removeClient(int i);
void removeSocket(int fd);
int pushCommand(char command, int op1, int op2);
int findClientFd(char* name);

int webSocket;
int localSocket;
char* unixPath;

int counter = 0;
int cN = 0;
Client clients[50];
int epoll;
pthread_t pinger;
pthread_t commander;
pthread_mutex_t clientsLock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv){
    srand((unsigned int)time(NULL));
    if(argc != 3) showUsage();
    if(signal(SIGINT, quitter) == SIG_ERR) print_error("Signal failed!");
    if(signal(SIGTERM, quitter) == SIG_ERR) print_error("Signal failed!");
    if(atexit(cleanup) != 0) print_error("At exit failed!"); // procedura sprzatajaca

    unsigned short portNum = validatePort((unsigned short)atoi(argv[1])); // walidacja poprawnosci inputu
    unixPath = validatePath(argv[2]);

    webSocket = getWebSocket(portNum); // adres, socket, bindowanie i listen
    localSocket = getLocalSocket(unixPath);

    epoll = prepareMonitor();

    if(pthread_create(&pinger, NULL, pingerJob, NULL) != 0) print_error("Creating thread failed!");
    if(pthread_create(&commander, NULL, commanderJob, NULL) != 0) print_error("Creating thread failed!");

    struct epoll_event event;
    while(1){
        if(epoll_wait(epoll, &event, 1, -1) == -1) print_error("Epoll_wait failed!");

        if(event.data.fd < 0){
            handleNewRequest(-event.data.fd);
        }else{ // nowa wiadomosc
            handleNewMessage(event.data.fd);
        }

    }
}
void handleNewRequest(int socket){
    int newClient = accept(socket, NULL, NULL);
    if(newClient == -1) print_error("accepting new client failed!");

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = newClient;

    if(epoll_ctl(epoll, EPOLL_CTL_ADD, newClient, &event) == -1) print_error("ctl add for new client failed!");
}

void handleNewMessage(int fd){
    char mType;
    short mLen;

    if(read(fd, &mType, 1) != 1) print_error("reading new message mType failed!");
    if(read(fd, &mLen, 2) != 2) print_error("reading new message mLen failed!");
    mLen = ntohs((uint16_t)mLen);
    char* name = malloc((size_t)mLen);

    if(mType == LOGIN){
        if(read(fd, name, (size_t)mLen) != mLen) print_error("reading new message login name failed!");
        tryRegister(fd, name);
    }else if(mType == RESULT){
        int resultCtn, result;
        if(read(fd, &resultCtn, sizeof(int)) != sizeof(int)) print_error("reading new message resultCtn failed!");
        resultCtn = ntohl((uint32_t)resultCtn);

        if(read(fd, &result, sizeof(int)) != sizeof(int)) print_error("reading new message result failed!");
        result = ntohl((uint32_t)result);

        if(read(fd, name, (size_t)mLen) != mLen) print_error("reading new message result name failed!");

        printf("Client %s calculated task %d with the result of %d!\n", name, resultCtn, result);
    }else if(mType == PONG){
        pthread_mutex_lock(&clientsLock); // zablokuj, zeby nie scigac sie z pingerem
        if(read(fd, name, (size_t)mLen) != mLen) print_error("reading new message pong name failed!"); // ten read moglby zfailowac jesli nas pinger wywlaszczyl
        for(int i=0; i<cN; i++){ // przeszukaj, czy pinger juz nie usunal tej nazwy
            if(strcmp(clients[i].name, name) == 0){
                clients[i].rec++; // jesli pinger jeszcze nie usunal, to zwieksz licznik otrzymanych
            }
        }
        pthread_mutex_unlock(&clientsLock);
    }else if(mType == LOGOUT){
        pthread_mutex_lock(&clientsLock); // zablokuj, zeby nie scigac sie z pingerem
        if(read(fd, name, (size_t)mLen) != mLen) print_error("reading logout name failed!"); // tak samo
        for(int i=0; i<cN; i++){
            if(strcmp(clients[i].name, name) == 0){ // faktycznie istnieje taki klient, ktory chcialby sie wylogowac
                printf("Client %s logged out!\n", name);
                removeClient(i);
            }
        }
        pthread_mutex_unlock(&clientsLock);
    }

    free(name);
}

void tryRegister(int fd, char* name){
    char mType;

    pthread_mutex_lock(&clientsLock);
    if(cN == 50){
        mType = FAILSIZE;
        if(write(fd, &mType, 1)!= 1) print_error("Failsize message failed!");
        removeSocket(fd);
    }else{
        int tmp = findClientFd(name);

        if(tmp != -1){
            mType = FAILNAME;
            if(write(fd, &mType, 1)!= 1) print_error("Failsize message failed!");
            removeSocket(fd);
        } else{
            clients[cN].fd = fd;
            clients[cN].name = concat("", name);
            clients[cN].sent = 0;
            clients[cN].rec = 0;
            cN++; // juz jest zarejestrowany w epollu - to wszystko
            mType = SUCCESS;
            if(write(fd, &mType, 1)!= 1) print_error("Success message failed!");
        }
    }
    pthread_mutex_unlock(&clientsLock);
}

void* pingerJob(void* arg){
    while(1){
        pthread_mutex_lock(&clientsLock);
        int j = 0;
        for(int i=0; i<cN; i++){
            if(clients[j].sent != clients[j].rec){
                printf("Woha! Removing %s!\n", clients[j].name);
                removeClient(j); // usuwam klienta i shiftuje tablice, wiec potem chce jeszcze raz ten sam indeks sprawzic
            }else{
                char mType = PING;
                if(write(clients[j].fd, &mType, 1)!= 1){
                    printf("There has been an error sending PING to client %s\n", clients[j].name);
                }
                clients[j].sent++;
                j++;
            }
        }
        pthread_mutex_unlock(&clientsLock);
        sleep(2);
    }
    return NULL;
}

void removeClient(int i){
    removeSocket(clients[i].fd);
    free(clients[i].name);
    for(int j = i; j + 1 < cN; j++){
        clients[j] = clients[j + 1];
    }
    cN--;
}

void removeSocket(int fd){
    if(epoll_ctl(epoll, EPOLL_CTL_DEL, fd, NULL) == -1) print_error("Removing from epoll failed!");
    if(shutdown(fd, SHUT_RDWR) == -1) print_error("Shutdowning failed!");
    if(close(fd) == -1) print_error("closing fd failed!");
}

void* commanderJob(void* arg){
    char command;
    int op1, op2;
    //char name[20];
    char buff[100];
    while(1){
        printf("Enter commands in format <op1> <command> <op2>!\n");
        fgets(buff, 100, stdin);
        if(sscanf(buff, "%d %c %d\n", &op1, &command, &op2) != 3){
            printf("Wrong format of input!\n");
            continue;
        }
        if((command != '+') && (command != '-') && (command != '/') && (command != '*')){
            printf("Wrong command!\n");
            continue;
        }

        int res = pushCommand(command, op1, op2); // zwraca id clienta do którego zostala wyslana operacja
        if(res >= 0){
            printf("Command %d: %d %c %d Has been sent to client %s!\n",counter -1, op1, command, op2, clients[res].name);
        }else if(res == -1){
            printf("Command couldnt be sent. No clients!\n");
        }else{
            printf("Command couldnt be sent. Something weird happened...\n");
        }
    }
    return NULL;
}

int pushCommand(char command, int op1, int op2){
    pthread_mutex_lock(&clientsLock); // lock po to, aby pinger i commander nie poplataly swoich wiadomosci -

    if(cN == 0){
        pthread_mutex_unlock(&clientsLock);
        return -1;
    }
    int id = rand()%cN;
    int fd = clients[id].fd;

    int ok = id;

    char mType = REQ;
    int currCtn = htonl((uint32_t)counter++);
    int ope1 = htonl((uint32_t)op1);
    int ope2 = htonl((uint32_t)op2);

    if(write(fd, &mType, 1) != 1) ok = -2;
    if(write(fd, &command, 1) != 1) ok = -2;
    if(write(fd, &currCtn, sizeof(int)) != sizeof(int)) ok = -2;
    if(write(fd, &ope1, sizeof(int)) != sizeof(int)) ok = -2;
    if(write(fd, &ope2, sizeof(int)) != sizeof(int)) ok = -2;
    pthread_mutex_unlock(&clientsLock);

    return ok;
}

int findClientFd(char* name){
    for(int i=0; i<cN; i++){
        if(strcmp(clients[i].name, name) == 0){
            return clients[i].fd;
        }
    }
    return -1;
}

int prepareMonitor(){
    int epoll = epoll_create1(0);
    if(epoll == -1) print_error("Creating epoll monitor failed!");

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = -webSocket;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, webSocket, &event) == -1) print_error("ctl for webSocket failed!");

    event.data.fd = -localSocket;
    if(epoll_ctl(epoll, EPOLL_CTL_ADD, localSocket, &event) == -1) print_error("ctl for localSocket failed!");

    return epoll;
}

int getWebSocket(short portNum){
    struct sockaddr_in webAddress;
    webAddress.sin_family = AF_INET;
    webAddress.sin_port = htons((uint16_t)portNum);
    webAddress.sin_addr.s_addr = INADDR_ANY;


    int webSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(webSocket == -1) print_error("Creating webSocket failed!");

    if(bind(webSocket, &webAddress, sizeof(webAddress)) == -1) print_error("webSocket binding failed!");
    if(listen(webSocket, 50) == -1) print_error("webSocket listen failed!");

    printf("Server address is %s\n", inet_ntoa(webAddress.sin_addr));

    return webSocket;

}

int getLocalSocket(char* path){
    struct sockaddr_un localAddress;
    localAddress.sun_family = AF_UNIX;
    for(int i=0; i<strlen(path) + 1; i++){
        localAddress.sun_path[i] = path[i];
    }

    int localSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(localSocket == -1) print_error("Creating localSocket failed!");

    if(bind(localSocket, &localAddress, sizeof(localAddress)) == -1) print_error("localSocket binding failed!");
    if(listen(localSocket, 50) == -1) print_error("localSocket listen failed!");

    return localSocket;
}

unsigned short validatePort(unsigned short port){
    if((port < 1024) || (port > 60999)){
        print_error("Port must be a number between 1024 and 60999!");
    }
    return port;
}

char* validatePath(char* path){
    int l = (int)strlen(path);
    if((l < 1) || (l > UNIX_PATH_MAX)){
        printf("Path must be of length between 1 and %d\n", UNIX_PATH_MAX);
        exit(1);
    }
    return path;
}

void quitter(int signo){
    exit(2);
}

void cleanup(void){
    int allOk = 1;

    if(close(webSocket) == -1){
        printf("Error closing webSocket! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if(close(localSocket) == -1){
        printf("Error closing localSocket! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if(unlink(unixPath) == -1){
        printf("Unlinking unixPath failed! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if(close(epoll) == -1){
        printf("Closing epoll failed! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }

    if(allOk == 1) printf("All cleaned up!\n");
}

void showUsage(){
    printf("Use program like <portNum> <unixPath>\n");
    exit(1);
}
