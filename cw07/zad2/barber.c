#define _GNU_SOURCE
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "FIFO.h"
#include "helpers.h"


int *salon;
int shmid;
sem_t *semid[3];


void initSHM(int seats){
    shmid = shm_open("FIFO_QUEUE",O_CREAT|O_EXCL|O_RDWR,0666);
    ftruncate(shmid,(seats+4)*sizeof(int));
    salon = (int*)mmap(NULL,(seats+4)*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,shmid,0);
}

void initSemaphores(){
    semid[BARBER] = sem_open("BARBER",O_CREAT|O_EXCL|O_RDWR,0666,0);
    semid[FIFO] = sem_open("FIFO",O_CREAT|O_EXCL|O_RDWR,0666,1);
    semid[SYNCHRONIZER] = sem_open("SYNCHRONIZER",O_CREAT|O_EXCL|O_RDWR,0666,0);
    //semctl(semid,SYNCHRONIZER,SETVAL,1);
}

void shave(int toShave){
    printf("Time: %ld, Barber: preparing to shave %d...\n",getMicroTime(),toShave);
    kill(toShave,SIGRTMIN);
    //sem_post(semid[SYNCHRONIZER]);
    printf("Time: %ld, Barber: shaving is finished, good bye %d\n",getMicroTime(),toShave);

}

void firstClientAfterNap(){
    sem_wait(semid[FIFO]);
    int toShave = getChair(salon);
    sem_post(semid[FIFO]);
    shave(toShave);
}

void cleaner(void){
    munmap(salon,salon[0]*sizeof(int));
    shm_unlink("FIFO_QUEUE");
    for(int i = 0; i < 3; i++){
        sem_close(semid[i]);

    }
    sem_unlink("BARBER");
    sem_unlink("FIFO");
    sem_unlink("SYNCHRONIZER");
}

void int_handler(int signo){
    exit(2);
}



void work(){
    int toShave;
    while(1){
        sem_wait(semid[BARBER]);
        firstClientAfterNap();
        while(1){
            sem_wait(semid[FIFO]);
            toShave = popFIFO(salon);

            if(toShave!=-1){
                shave(toShave);
                sem_post(semid[FIFO]);
            }
            else{
                printf("Time: %ld, Barber: I'm so tired so I take a nap...\n",getMicroTime());
                sem_wait(semid[BARBER]);
                sem_post(semid[FIFO]);
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
    initSemaphores();
    initFIFO(salon,seats);

    work();


    return 0;
}