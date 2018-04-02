//
// Created by kwrobel on 05.05.17.
//

#include "helpers.h"
#include <time.h>
#include <sys/sem.h>

long getMicroTime(){
    struct timespec marker;
    clock_gettime(CLOCK_MONOTONIC, &marker);
    return marker.tv_nsec;
}
int upSem(int semid, semaphoreType sType){
    struct sembuf sops;
    sops.sem_num=sType;
    sops.sem_op=1;
    sops.sem_flg=0;

    return semop(semid,&sops, 1);

}

int downSem(int semid, semaphoreType sType){
    struct sembuf sops;
    sops.sem_num=sType;
    sops.sem_op=-1;
    sops.sem_flg=0;

    return semop(semid,&sops, 1);
}
