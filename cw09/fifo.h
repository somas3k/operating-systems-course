//
// Created by kwrobel on 22.05.17.
//

#ifndef LAB9_V2_FIFO_H
#define LAB9_V2_FIFO_H

#include <sys/param.h>

typedef struct thread_info{
    int id;
    int function;
} thread_info;

typedef struct thread{
    thread_info info;
    struct thread *next;
} thread;

typedef struct fifo{
    thread *first;
    thread *last;
} fifo;

fifo *fifo_init();

thread_info fifo_pop(fifo *f);

void fifo_push(fifo *f, thread_info info);

int fifo_isEmpty(fifo *f);

void free_fifo(fifo *f);

#endif //LAB9_V2_FIFO_H
