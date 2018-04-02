#define _GNU_SOURCE
#include <stdio.h>
#include "common.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <mqueue.h>
#include <signal.h>


struct client_info{
    int pid;
    int queue_id;
};

struct client_info clients[MAX_CLIENTS];
int clients_count = 0;

int server;

mqd_t connectToClient(int client_pid){
    mqd_t client;

    char client_path[10];
    sprintf(client_path,"/%d",client_pid);

    if((client=mq_open(client_path,O_WRONLY))==-1){
        perror("get ");
        perror(strerror(errno));
    }
    return client;
}

int addClient(){
    if(clients_count==MAX_CLIENTS) return -1;
    int i=0;
    while(clients[i].pid!=0){ i++;}

    clients_count++;
    return i;
}

void deleteClient(int id){
    clients[id].pid=0;
    clients_count--;
}

void sendClientID(int client){
    char response[MAX_LENGTH_MSG];
    sprintf(response,"%d|%d|%d",ID,client,getpid());

    if(mq_send(clients[client].queue_id,response,MAX_LENGTH_MSG,0)==-1){
        perror("msgsnd_in_sendClientID");
        exit(EXIT_FAILURE);
    }
    mq_close(clients[client].queue_id);
}

int openMainQueue(){
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_LENGTH_MSG;
    attr.mq_curmsgs = 0;
    mqd_t server = mq_open(SERVER,O_CREAT|O_RDWR,0644,&attr);

    if (server < 0) {
        perror(strerror(errno));
        printf("failed to create message queue with msgqid = %d\n", server);
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<MAX_CLIENTS;++i){
        clients[i].pid = 0;
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

void int_handler(int signo){
    exit(0);
}

void endFunction(void){
    mq_close(server);
    mq_unlink(SERVER);
}

int main() {
    signal(SIGINT,int_handler);
    atexit(endFunction);

    printf("Server with pid %d\n",getpid());

    server = openMainQueue();

    printf("Server opened a message queue with id %d\n",server);

    char buffor[MAX_LENGTH_MSG];
    memset(buffor,'\0',MAX_LENGTH_MSG);
    char response[MAX_LENGTH_MSG];
    char client_path[10];

    while(1){
        if(mq_receive(server,buffor,MAX_LENGTH_MSG,NULL)>=0){
            int type = atoi(strtok(buffor,"|"));

            if(type==LOGIN){
                int client_pid = atoi(strtok(NULL,"|"));
                printf("Process %d is trying to connect to server...\n",client_pid);
                mqd_t client = connectToClient(client_pid);
                if(client!=-1) {
                    printf("Process %d connected to server. Adding client to client table...\n",client_pid);
                    int id = addClient();
                    if (id != -1) {
                        clients[id].pid = client_pid;
                        clients[id].queue_id=client;
                        sendClientID(id);
                        printf("Process %d added as a client with id %d\n",client_pid,id);
                    }
                    else{
                        printf("Process %d didn't add as a client\n",client_pid);
                        sprintf(response,"%d|%d|Too many clients now :(. Try again later.",ERROR,getpid());
                        mq_send(client,response,MAX_LENGTH_MSG,0);
                        mq_close(client);
                    }
                }
            }
            else {
                if (type == EXIT_SERVER) {
                    exit(0);
                }
                else {
                    int id = atoi(strtok(NULL, "|"));
                    sprintf(client_path, "/%d", clients[id].pid);
                    mqd_t queue_id = mq_open(client_path, O_WRONLY);
                    if (type == ECHO) {
                        printf("Client %d send ECHO order\n", id);
                        sprintf(response, "%s", strtok(NULL, "\0"));
                        mq_send(queue_id, response, MAX_LENGTH_MSG, 0);
                    }
                    if (type == UPPER) {
                        printf("Client %d send UPPER order\n", id);
                        sprintf(response, "%s", strtok(NULL, "\0"));
                        toCapitals(response);
                        mq_send(queue_id, response, MAX_LENGTH_MSG, 0);
                    }
                    if (type == TIME) {
                        printf("Client %d send TIME order\n", id);
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        sprintf(response, "now: %d-%d-%d %d:%d:%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                                tm.tm_hour,
                                tm.tm_min, tm.tm_sec);
                        mq_send(queue_id, response, MAX_LENGTH_MSG, 0);
                    }
                    if (type == LOGOUT) {
                        printf("Client %d send LOGOUT order\n", id);
                        sprintf(response, "Disconnected successfully");
                        deleteClient(id);
                        mq_send(queue_id, response, MAX_LENGTH_MSG, 0);
                    }
                    if (type == QUIT) {
                        printf("Client %d send QUIT order\n", id);
                        sprintf(response, "%d", EXIT_SERVER);
                        mq_send(server, response, MAX_LENGTH_MSG, 0);
                    }

                    mq_close(queue_id);
                }

            }

        }

    }
}
