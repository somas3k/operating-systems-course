#define _GNU_SOURCE

#include <stdio.h>
#include "common.h"
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <unistd.h>

int fromServer;
int toServer;
int id;

void receiveMessage(struct bufor *rec){
    if(msgrcv(fromServer, rec, MSG_SIZE, 0, 0) < 0) {
        fprintf(stderr,"Msgrcv error");
        exit(1);
    }
}

void createQueueConnToServ(){
    key_t client;
    printf("Trying create message queue...\n");
    do{
        client = ftok(getenv("HOME"),getKey());
    } while((fromServer = msgget(client,IPC_EXCL | IPC_CREAT | 0600))==-1);

    printf("Private message queue created successfully with id %d\n",fromServer);
    printf("Connecting to server message queue...\n");
    toServer = msgget(ftok(getenv("HOME"),SERVER),IPC_CREAT|0600);
    if (toServer < 0) {
        perror(strerror(errno));
        printf("failed to create message queue with msgqid = %d\n", toServer);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server message queue.\n");
    printf("Trying to connect as a client...\n");
    struct bufor buf;
    buf.type=LOGIN;
    buf.pid=getpid();
    sprintf(buf.msg,"%d",client);

    if(msgsnd(toServer,&buf,MSG_SIZE,0)!=-1) {
        msgrcv(fromServer,&buf,MSG_SIZE,0,0);
        if(buf.type==ID) {
            id = buf.id;
            printf("Connected to server with pid %d as a client with id %d\n", buf.pid, id);
        }
        if(buf.type==ERROR){
            printf("%s\n",buf.msg);
            exit(0);
        }
    }
    else {
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void endFunction(void){
    msgctl(fromServer,IPC_RMID,NULL);
}

int main() {

    atexit(endFunction);
    printf("Client with pid %d\n",getpid());

    createQueueConnToServ();

    struct bufor buf;
    struct bufor rec;
    buf.id=id;
    buf.pid=getpid();
    char *line = NULL;
    size_t size = 0;
    while(1){
        printf("Enter order type( 1 - ECHO, 2 - UPPER, 3 - TIME, 4 - LOGOUT, 5 - QUIT): \n");
        ssize_t charsRead = getline(&line,&size,stdin);
        if(charsRead!=-1) {
            switch (atoi(line)) {
                case ECHO :
                    printf("Podaj tresc wiadomosci:\n");
                    fgets(buf.msg, MAX_LENGTH_MSG, stdin);
                    buf.type = ECHO;
                    msgsnd(toServer, &buf, MSG_SIZE, 0);
                    receiveMessage(&rec);
                    printf("%s\n", rec.msg);
                    break;
                case UPPER :
                    printf("Podaj tresc wiadomosci:\n");
                    fgets(buf.msg, MAX_LENGTH_MSG, stdin);
                    buf.type = UPPER;
                    msgsnd(toServer, &buf, MSG_SIZE, 0);
                    receiveMessage(&rec);
                    printf("%s\n", rec.msg);
                    break;
                case TIME:
                    buf.type = TIME;
                    msgsnd(toServer, &buf, MSG_SIZE, 0);
                    receiveMessage(&rec);
                    printf("%s\n", rec.msg);
                    break;
                case LOGOUT:
                    buf.type = LOGOUT;
                    msgsnd(toServer, &buf, MSG_SIZE, 0);
                    receiveMessage(&rec);
                    printf("%s\n", rec.msg);
                    exit(0);
                case QUIT:
                    buf.type = QUIT;
                    msgsnd(toServer, &buf, MSG_SIZE, 0);
                    exit(0);
                default:
                    printf("Something wrong with entered option!!\n");
                    break;
            }
        }
    }
}
