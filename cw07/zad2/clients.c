#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include "FIFO.h"
#include "helpers.h"

int shavesCounter = 0;
int shavesNumber;

int *salon;
int shmid;
sem_t *semid[3];
sigset_t fullMask;

void cleaner(void){
    munmap(salon,salon[0]*sizeof(int));
    sem_close(semid[BARBER]);
    sem_close(semid[FIFO]);
    sem_close(semid[SYNCHRONIZER]);
}

void int_handler(int signo){
    exit(2);
}

void rtmin_handler(int signo){
   shavesCounter++;
}

void connectToSHM(){
    shmid = shm_open("FIFO_QUEUE",O_RDWR,0666);
    int *tmp = (int*)mmap(NULL,sizeof(int),PROT_READ,MAP_SHARED,shmid,0);
    int N = tmp[0];
    munmap(tmp,sizeof(int));
    salon = (int*)mmap(NULL,N*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,shmid,0);
}

void connectToSemaphores(){
    semid[BARBER] = sem_open("BARBER",O_RDWR,0666);
    semid[FIFO] = sem_open("FIFO",O_RDWR,0666);
    semid[SYNCHRONIZER] = sem_open("SYNCHRONIZER",O_RDWR,0666);
}

void maskSignals(){
    sigfillset(&fullMask);
    sigdelset(&fullMask,SIGINT);
    sigdelset(&fullMask,SIGRTMIN);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGRTMIN);
    sigprocmask(SIG_BLOCK,&mask,NULL);
}

int takeASeat(){
    int barberStatus;
    sem_getvalue(semid[BARBER],&barberStatus);
    if(barberStatus==0){
        sem_post(semid[BARBER]); //wake up
        sem_post(semid[BARBER]); //tell others than barber is working now
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
        //sem_wait(semid[SYNCHRONIZER]);
        sem_wait(semid[FIFO]);
        int result = takeASeat();
        sem_post(semid[FIFO]);
        //sem_post(semid[SYNCHRONIZER]);
        if(result == 0){
            sigsuspend(&fullMask);
            //sem_wait(semid[SYNCHRONIZER]);
            //shavesCounter++;
            printf("Time: %ld, Client %d just got shave\n",getMicroTime(),getpid());
        }

    }
}

int main(int argc, char **argv) {
    if(argc!=3) exit(EXIT_FAILURE);
    atexit(cleaner);
    signal(SIGINT,int_handler);
    signal(SIGRTMIN,rtmin_handler);

    int clientNum = atoi(argv[1]);

    shavesNumber = atoi(argv[2]);

    connectToSHM();
    connectToSemaphores();


    maskSignals();
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
