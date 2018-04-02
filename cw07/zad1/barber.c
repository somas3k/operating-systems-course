#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include "FIFO.h"
#include "helpers.h"

int *salon;
int shmid;
int semid;


void initSHM(int seats){
    shmid = shmget(ftok(getenv("HOME"),'b'),(seats+4)*sizeof(int),IPC_CREAT|IPC_EXCL|0600);
    salon = (int*)shmat(shmid,NULL,0);
}

void initSemaphores(int count){
    semid = semget(ftok(getenv("HOME"),'b'),count,IPC_CREAT|IPC_EXCL|0600);
    semctl(semid,BARBER,SETVAL,0);
    semctl(semid,FIFO,SETVAL,1);
    semctl(semid,SYNCHRONIZER,SETVAL,0);
    //semctl(semid,SYNCHRONIZER,SETVAL,1);
}

void shave(int toShave){
    printf("Time: %ld, Barber: preparing to shave %d...\n",getMicroTime(),toShave);
    //kill(toShave,SIGRTMIN);
    upSem(semid,SYNCHRONIZER);
    printf("Time: %ld, Barber: shaving is finished, good bye %d\n",getMicroTime(),toShave);

}

void firstClientAfterNap(){
    downSem(semid,FIFO);
    int toShave = getChair(salon);
    upSem(semid,FIFO);
    shave(toShave);
}

void cleaner(void){
    shmdt(salon);
    shmctl(shmid,IPC_RMID,NULL);
    semctl(semid,0,IPC_RMID);
}

void int_handler(int signo){
    exit(2);
}



void work(){
    int toShave;
    while(1){
        downSem(semid,BARBER);
        firstClientAfterNap();
        while(1){
            downSem(semid,FIFO);
            toShave = popFIFO(salon);

            if(toShave!=-1){
                shave(toShave);
                upSem(semid,FIFO);
            }
            else{
                printf("Time: %ld, Barber: I'm so tired so I take a nap...\n",getMicroTime());
                downSem(semid,BARBER);
                upSem(semid,FIFO);
                break;
            }
        }
    }
}


int main(int argc, char **argv) {
    if(argc!=2){
        exit(EXIT_FAILURE);
    }
    printf("PID: %d\n", getpid());
    atexit(cleaner);
    signal(SIGINT,int_handler);

    int seats = atoi(argv[1]);
    initSHM(seats);
    initSemaphores(3);
    initFIFO(salon,seats);

    work();


    return 0;
}