#define _GNU_SOURCE

#include <stdio.h>
#include "common.h"
#include <sys/msg.h>
#include <errno.h>
#include <memory.h>
#include <ctype.h>
#include <unistd.h>

key_t clients[MAX_CLIENTS];
int clients_count = 0;

int server;

int connectToClient(int client_key){
    key_t client;

    if((client=msgget(client_key,IPC_CREAT|0600))==-1){
        perror("get ");
        perror(strerror(errno));
    }
    return client;
}

int addClient(){
    if(clients_count==MAX_CLIENTS) return -1;
    int i=0;
    while(clients[i]!=0){ i++;}
    clients_count++;
    return i;
}

int deleteClient(int id){
    int client = clients[id];
    clients[id]=0;
    clients_count--;
    return client;
}

void sendClientID(int client){
    struct bufor buf;
    buf.type=ID;
    buf.id=client;
    buf.pid = getpid();
    sprintf(buf.msg,"ClientID");

    if(msgsnd(clients[client],&buf,MSG_SIZE,0)==-1){
        perror("msgsnd_in_sendClientID");
        exit(EXIT_FAILURE);
    }
}

int openMainQueue(){
    int server = msgget(ftok(getenv("HOME"),SERVER),IPC_CREAT|0600);

    if (server < 0) {
        perror(strerror(errno));
        printf("failed to create message queue with msgqid = %d\n", server);
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<MAX_CLIENTS;++i){
        clients[i] = 0;
    }

    return server;
}

void toCapitals(char msg[MAX_LENGTH_MSG]){
    int i =0;
    while(msg[i]!='\0'&&i<MAX_LENGTH_MSG){
        msg[i]=(char)toupper(msg[i]);
        i++;
    }
}

void endFunction(void){
    msgctl(server,IPC_RMID,NULL);
}

int main() {

    atexit(endFunction);

    printf("Server with pid %d\n",getpid());

    server = openMainQueue();

    printf("Server opened a message queue with id %d\n",server);

    struct bufor buf;

    while(1){
        if(msgrcv(server,&buf,MSG_SIZE,0,0)>=0){
            if(buf.type==LOGIN){
                printf("Process %d is trying to connect to server...\n",buf.pid);
                int client = connectToClient(atoi(buf.msg));
                if(client!=-1) {
                    printf("Process %d connected to server. Adding client to client table...\n",buf.pid);
                    int id = addClient();
                    if (id != -1) {
                        clients[id] = client;
                        sendClientID(id);
                        printf("Process %d added as a client with id %d\n",buf.pid,id);
                    }
                    else{
                        printf("Process %d didn't add as a client\n",buf.pid);
                        buf.type=ERROR;
                        buf.pid = getpid();
                        sprintf(buf.msg,"Too many clients now :(. Try again later.");
                        msgsnd(client,&buf,MSG_SIZE,0);

                    }
                }
                else{
                    buf.type=ERROR;
                    msgsnd(client,&buf,MSG_SIZE,0);
                }
            }
            buf.pid = getpid();
            if(buf.type==ECHO){
                printf("Client %d send ECHO order\n",buf.id);
                msgsnd(clients[buf.id],&buf,MSG_SIZE,0);
            }
            if(buf.type==UPPER){
                printf("Client %d send UPPER order\n",buf.id);
                toCapitals(buf.msg);
                sprintf(buf.msg,"%s",buf.msg);
                msgsnd(clients[buf.id],&buf,MSG_SIZE,0);
            }
            if(buf.type==TIME){
                printf("Client %d send TIME order\n",buf.id);
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                sprintf(buf.msg,"now: %d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                msgsnd(clients[buf.id],&buf,MSG_SIZE,0);
            }
            if(buf.type==LOGOUT){
                printf("Client %d send LOGOUT order\n",buf.id);
                sprintf(buf.msg,"Disconnected successful");
                msgsnd(deleteClient(buf.id),&buf,MSG_SIZE,0);
            }
            if(buf.type==QUIT){
                printf("Client %d send QUIT order\n",buf.id);
                buf.type=EXIT_SERVER;
                msgsnd(server,&buf,MSG_SIZE,0);
            }
            if(buf.type==EXIT_SERVER){
                exit(0);
            }

        }

    }
}
