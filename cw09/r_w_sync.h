//
// Created by kwrobel on 22.05.17.
//

#ifndef LAB9_V2_MAIN_H
#define LAB9_V2_MAIN_H

#include "fifo.h"
#include <stdio.h>
#include <semaphore.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define TAB_SIZE 10

#define READER 0
#define WRITER 1

int data[TAB_SIZE];

int info = 0;

typedef struct args{
    int readers;
    int writers;
} args;

typedef struct reader_arg{
    int id;
    int divider;
} reader_arg;

void exit_handler();
void init_sync_helpers();

void sigint_handler(int signo);

void *writer_thread_fun(void *arg);

void *reader_thread_fun(void *arg);

args *parse_args(int argc, char **argv);

void generate_data();

#endif //LAB9_V2_MAIN_H
