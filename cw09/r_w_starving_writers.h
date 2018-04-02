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

typedef struct args{
    int readers;
    int writers;
} args;

typedef struct reader_arg{
    int divider;
} reader_arg;

void *writer_thread_fun(void *arg);

void *reader_thread_fun(void *arg);

args *parse_args(int argc, char **argv);

void generate_data();
