#define X_OPEN_SOURCE 1000
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

int Type;

int sent_sig_to_child;
int rec_sig_from_child;
int rec_sig_by_child;

int pid;

void sigint_handler(int signo){
    kill(pid,SIGINT);
    exit(EXIT_SUCCESS);
}

void sigs_par_handler(int signo){
    if(Type==1) signal(SIGUSR1,sigs_par_handler);
    if(Type==3) signal(SIGRTMIN,sigs_par_handler);

    rec_sig_from_child++;
}

void sigs_child_handler(int signo){

    if(signo==SIGUSR2 || signo==SIGRTMAX){
        _exit(rec_sig_by_child);
    }
    kill(getppid(),signo);
    rec_sig_by_child++;

    if(Type==1) {
        signal(SIGUSR1, sigs_child_handler);
        signal(SIGUSR2, sigs_child_handler);
    }
    if(Type==3) {
        signal(SIGRTMIN, sigs_child_handler);
        signal(SIGRTMAX, sigs_child_handler);
    }


}

int main(int argc, char **argv) {
    int L = atoi(argv[1]);
    Type = atoi(argv[2]);

    union sigval sigval1;
    sigval1.sival_int=getpid();

    sent_sig_to_child=0;
    rec_sig_from_child=0;

    struct sigaction action;
    action.sa_flags = 0;

    pid = fork();

    if(pid==0){
        action.sa_handler=sigs_child_handler;
        rec_sig_by_child=0;


        if(Type==1) {
            signal(SIGUSR1, sigs_child_handler);
            signal(SIGUSR2, sigs_child_handler);
        }
        if(Type==2) {
            sigaction(SIGUSR1, &action, NULL);
            sigaction(SIGUSR2, &action, NULL);
        }
        if(Type==3) {
            signal(SIGRTMIN, sigs_child_handler);
            signal(SIGRTMAX, sigs_child_handler);
        }
        while(1);

    }
    else{
        signal(SIGINT,sigint_handler);
        action.sa_handler=sigs_par_handler;
        int i;
        if(Type==1) signal(SIGUSR1,sigs_par_handler);
        if(Type==2) sigaction(SIGUSR1,&action,NULL);
        if(Type==3) signal(SIGRTMIN,sigs_par_handler);
        sleep(1);
        for(i=0;i<L;i++){
            sent_sig_to_child++;
            if(Type==1) kill(pid,SIGUSR1);
            if(Type==2) sigqueue(pid,SIGUSR1,sigval1);
            if(Type==3) kill(pid,SIGRTMIN);
            sleep(1);
        }
        if(Type==1) kill(pid,SIGUSR2);
        if(Type==2) sigqueue(pid,SIGUSR2,sigval1);
        if(Type==3) kill(pid,SIGRTMAX);
    }


    int status=0;

    wait(&status);

    printf("Count of signals sent to child: %d\n", sent_sig_to_child);
    printf("Count of signals received by child: %d\n", WEXITSTATUS(status));
    printf("Count of signals received from child: %d\n", rec_sig_from_child);

    return 0;
}
