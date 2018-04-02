#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include "FIFO.h"
#include "helpers.h"

int shavesCounter = 0;
int shavesNumber;

int *salon;
int shmid;
int semid;
//sigset_t fullMask;

void cleaner(void){
    shmdt(salon);
}

void int_handler(int signo){
    exit(2);
}

//void rtmin_handler(int signo){
//    shavesCounter++;
//}

void connectToSHM(){
    shmid = shmget(ftok(getenv("HOME"),'b'),0,0);
    salon = (int*)shmat(shmid,NULL,0);
}

void connectToSemaphores(){
    semid = semget(ftok(getenv("HOME"),'b'),0,0);
}

/*void maskSignals(){
    sigfillset(&fullMask);
    sigdelset(&fullMask,SIGINT);
    sigdelset(&fullMask,SIGRTMIN);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGRTMIN);
    sigprocmask(SIG_BLOCK,&mask,NULL);
}*/

int takeASeat(){
    int barberStatus = semctl(semid,BARBER,GETVAL);
    if(barberStatus==0){
        upSem(semid,BARBER); //wake up
        upSem(semid,BARBER); //tell others than barber is working now
        printf("Time: %ld, Client %d woke up barber \n",getMicroTime(),getpid());
        seatOnChair(salon,getpid());
        return 0;
    }
    else{
        int result = pushFIFO(getpid(),salon);
        if(result==-1){
            printf("Time: %ld, Client %d couldn't find free place to wait\n",getMicroTime(),getpid());
            return -1;
        }
        else{
            printf("Time: %ld, Client %d took a place in queue\n",getMicroTime(),getpid());
            return 0;
        }
    }

}

void takeShaves(){
    while(shavesCounter<shavesNumber){
        //downSem(semid,SYNCHRONIZER);
        downSem(semid,FIFO);
        int result = takeASeat();
        upSem(semid,FIFO);
        //upSem(semid,SYNCHRONIZER);
        if(result == 0){
            //sigsuspend(&fullMask);
            downSem(semid,SYNCHRONIZER);
            shavesCounter++;
            printf("Time: %ld, Client %d just got shave\n",getMicroTime(),getpid());
        }

    }
}

int main(int argc, char **argv) {
    if(argc!=3) exit(EXIT_FAILURE);
    atexit(cleaner);
    signal(SIGINT,int_handler);
    //signal(SIGRTMIN,rtmin_handler);

    int clientNum = atoi(argv[1]);

    shavesNumber = atoi(argv[2]);

    connectToSHM();
    connectToSemaphores();


    //maskSignals();
    //printf("clientNum: %d\n",clientNum);


    for(int i = 0; i < clientNum; i++){
        pid_t pid = fork();
        if(pid==0){
            //printf("PID: %d\n", getpid());
            takeShaves();
            exit(0);
        }

    }
    int i = 0;
    int status;
    while(i < clientNum){
        wait(&status);

        i++;
    }

    return 0;
}
