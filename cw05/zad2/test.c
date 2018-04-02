//
// Created by kwrobel on 11.04.17.
//


#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc!=4){
        perror("Wrong number of arguments\n");
        exit(EXIT_FAILURE);
    }
    pid_t master=fork();
    if(master==0) execlp("./master","master","pipe",argv[1],NULL);
    sleep(1);
    for(int i=0;i<10;i++){
        if(fork()==0) execlp("./slave","slave","pipe",argv[2],argv[3],NULL);
    }

    int status;
    while(wait(&status)!=-1);

    return 0;
}

