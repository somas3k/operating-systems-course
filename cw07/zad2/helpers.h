//
// Created by kwrobel on 05.05.17.
//

#ifndef LAB7_HELPERS_H
#define LAB7_HELPERS_H

typedef enum semaphoreType{
    BARBER = 0, FIFO = 1, SYNCHRONIZER = 2
} semaphoreType;

long getMicroTime();

/*int upSem(int semid, semaphoreType sType);

int downSem(int semid, semaphoreType sType);*/

#endif //LAB7_HELPERS_H
