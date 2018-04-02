#include <stdio.h>
#include "common.h"
#include <errno.h>
#include <memory.h>
#include <unistd.h>
#include <mqueue.h>
#include <signal.h>

mqd_t fromServer;
mqd_t toServer;
int id;

void receiveMessage(char *buffer){
    if(mq_receive(fromServer, buffer, MAX_LENGTH_MSG,NULL) < 0) {
        fprintf(stderr,"Msgrcv error");
        exit(1);
    }
}

void int_handler(int signo){
    exit(0);
}

void createQueueConnToServ(){

    printf("Trying create message queue...\n");
    char queue_path[10];
    memset(queue_path,'\0',10);
    sprintf(queue_path,"/%d",getpid());
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_LENGTH_MSG;
    attr.mq_curmsgs = 0;
    fromServer = mq_open(queue_path,O_RDONLY|O_CREAT,0644,&attr);
    printf("Private message queue created successfully with id %d\n",fromServer);
    printf("Connecting to server message queue...\n");
    toServer = mq_open(SERVER,O_WRONLY);
    if (toServer < 0) {
        perror(strerror(errno));
        printf("failed to create message queue with msgqid = %d\n", toServer);
        exit(EXIT_FAILURE);
    }
    char login[MAX_LENGTH_MSG];
    printf("Connected to server message queue.\n");
    printf("Trying to connect as a client...\n");
    memset(login,'\0',MAX_LENGTH_MSG);
    sprintf(login,"%d|%d",LOGIN,getpid());

    if(mq_send(toServer,login,MAX_LENGTH_MSG,0)>=0) {
        memset(login,'\0',MAX_LENGTH_MSG);
        mq_receive(fromServer,login,MAX_LENGTH_MSG,NULL);
        int type = atoi(strtok(login,"|"));
        if(type==ID) {
            id = atoi(strtok(NULL,"|"));
            printf("Connected to server with pid %d as a client with id %d\n", atoi(strtok(NULL,"\0")), id);
        }
        if(type==ERROR){
            printf("Server with pid %d: %s\n",atoi(strtok(NULL,"|")),strtok(NULL,"\0"));
            exit(0);
        }
    }
    else {
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void endFunction(void){
    mq_close(fromServer);
    char queue_path[10];
    memset(queue_path,'\0',10);
    sprintf(queue_path,"/%d",getpid());
    mq_unlink(queue_path);
}

int main() {

    atexit(endFunction);
    printf("Client with pid %d\n",getpid());
    signal(SIGINT,int_handler);

    createQueueConnToServ();

    char buffer[MAX_LENGTH_MSG];
    char text[MAX_LENGTH_MSG];




    char *line = NULL;
    size_t size = 0;
    while(1){
        printf("Enter order type( 1 - ECHO, 2 - UPPER, 3 - TIME, 4 - LOGOUT, 5 - QUIT): \n");
        ssize_t charsRead = getline(&line,&size,stdin);
        if(charsRead!=-1) {
            switch (atoi(line)) {
                case ECHO :
                    printf("Podaj tresc wiadomosci:\n");
                    fgets(text, MAX_LENGTH_MSG, stdin);
                    sprintf(buffer,"%d|%d|%s",ECHO,id,text);
                    mq_send(toServer,buffer,MAX_LENGTH_MSG,0);
                    receiveMessage(buffer);
                    printf("%s\n", buffer);
                    break;
                case UPPER :
                    printf("Podaj tresc wiadomosci:\n");
                    fgets(text, MAX_LENGTH_MSG, stdin);
                    sprintf(buffer,"%d|%d|%s",UPPER,id,text);
                    mq_send(toServer,buffer,MAX_LENGTH_MSG,0);
                    receiveMessage(buffer);
                    printf("%s\n", buffer);
                    break;
                case TIME:
                    sprintf(buffer,"%d|%d",TIME,id);
                    mq_send(toServer,buffer,MAX_LENGTH_MSG,0);
                    receiveMessage(buffer);
                    printf("%s\n", buffer);
                    break;
                case LOGOUT:
                    sprintf(buffer,"%d|%d",LOGOUT,id);
                    mq_send(toServer,buffer,MAX_LENGTH_MSG,0);
                    receiveMessage(buffer);
                    printf("%s\n", buffer);
                    exit(0);
                case QUIT:
                    sprintf(buffer,"%d|%d",QUIT,id);
                    mq_send(toServer,buffer,MAX_LENGTH_MSG,0);
                    exit(0);
                default:
                    printf("Something wrong with entered option!!\n");
                    break;
            }
        }
    }
}
