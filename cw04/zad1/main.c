#define _XOPEN_SOURCE 1000
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int direction;
void sig_handler(int signo){
    if(signo==SIGINT){
        printf("Odebrano sygnal SIGINT\n");
        exit(0);
    }
    if(signo==SIGTSTP){
        if(direction==1) direction=0;
        else direction=1;
        signal(SIGTSTP,sig_handler);
    }
}

int main() {
    char alphabet[26]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i=0;
    direction=1;
    struct sigaction action;
    action.sa_handler=sig_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags=0;
    sigaction(SIGINT,&action,NULL);

    signal(SIGTSTP,sig_handler);


    while(1){
        if(direction==1){
            printf("%c\n",alphabet[i++]);
            if(i==26) i=0;
            sleep(1);
        }
        else{
            printf("%c\n",alphabet[i--]);
            if(i==-1) i=25;
            sleep(1);
        }
    }
}
