#define _XOPEN_SOURCE 1000
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>

int N;
int M;
int K;
struct child_info{
    int pid;
    int request;
    int exit_code;
    int id;
    int sigrt;
} *childInfo;

int permission;

void sigint_handler(int signo){
    int i;
    for(i=0;i<N;i++){
        kill(childInfo[i].pid,SIGINT);
    }

    exit(EXIT_SUCCESS);
}

void sigrt_handler(int signo, siginfo_t *info, void *extra){
    int i=0;
    while (childInfo[i].pid != info->si_value.sival_int) i++;
    childInfo[i].sigrt=signo;

    struct sigaction action;
    action.sa_flags=SA_SIGINFO;
    action.sa_sigaction=sigrt_handler;


    sigaction(signo,&action,NULL);

}

void sigusr2_handler(int signo){
    permission=1;
}
void sigusr1_handler(int signo, siginfo_t *info,void *extra){

    int i=0;
    K++;

    if(K<M) {
        while (childInfo[i].pid != info->si_value.sival_int) i++;
        childInfo[i].request = 1;
    }
    else {
        if (K == M) {
            while (childInfo[i].pid != info->si_value.sival_int) i++;
            childInfo[i].request = 1;
            for (i = 0; i < N; i++) {
                if (childInfo[i].request == 1) {
                    kill(childInfo[i].pid, SIGUSR2);
                }
            }
        }
        else {
            kill(info->si_value.sival_int, SIGUSR2);
        }
    }

}

int main(int argc, char** argv) {

    N = atoi(argv[1]);
    M = atoi(argv[2]);
    K=0;
    struct sigaction action;
    action.sa_flags=SA_SIGINFO | SA_NODEFER;
    action.sa_sigaction=sigusr1_handler;

    sigaction(SIGUSR1,&action,NULL);

    action.sa_flags=SA_SIGINFO;
    action.sa_sigaction=sigrt_handler;

    signal(SIGINT,sigint_handler);

    int i=0;

    for(i=0;i<=32;i++){
        sigaction(SIGRTMIN+i,&action,NULL);
    }


    childInfo=malloc(N*sizeof(struct child_info));
    int pid;
    for(i=0; i<N; i++){
        childInfo[i].request=0;
        childInfo[i].exit_code=-1;
        childInfo[i].id=i;
        pid=fork();
        childInfo[i].pid=pid;

        if(pid==0){
            permission=0;
            signal(SIGUSR2,sigusr2_handler);
            srand((unsigned)(time(0) ^ (getpid() << 16)));

            int ppid=getppid();
            union sigval sigval1;
            sigval1.sival_int=getpid();

            unsigned rand_val=(unsigned)rand()%11;
            sleep(rand_val);

            sigqueue(ppid,SIGUSR1,sigval1);
            clock_t start = clock();

            while(permission==0){
                if((clock()-start)/CLOCKS_PER_SEC>10){
                    exit(1);
                }
            }

            sigqueue(ppid,SIGRTMIN+rand()%33,sigval1);
            clock_t end = clock();
            exit((int)(end-start));
        }
        else{

        }
    }

    int counter=0;
    int status=0;

    while(counter != N){
        pid = wait(&status);
        if(pid!=-1){
            i=0;
            while (childInfo[i].pid != pid) i++;
            childInfo[i].exit_code=WEXITSTATUS(status);
            counter++;
        }
    }
    counter = 0;
    for(i=0;i<N;i++){
        if(childInfo[i].sigrt==0) counter++;
        printf("ID: %d, PID: %d, RT: %d, TIME: %d\n",childInfo[i].id,childInfo[i].pid,childInfo[i].sigrt,childInfo[i].exit_code);
    }
    printf("Count of process with unhandled signals: %d\n", counter);
    return 0;

}