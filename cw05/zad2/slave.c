#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

double rand_re(){
    return -2.0 + 3*((double)rand()/(double)RAND_MAX);
}

double rand_im(){
    return -1.0 + 2*((double)rand()/(double)RAND_MAX);
}

int main(int argc, char **argv) {
    if(argc != 4){
        perror("Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }
    char *pipe_name= argv[1];
    int N = atoi(argv[2]);
    int K = atoi(argv[3]);

    srand(((unsigned )time(NULL)) ^ ((unsigned )getpid()<<16));

    int pipe = open(pipe_name,O_WRONLY | S_IFIFO);
    if(pipe==-1) {
        perror("Pipe doesn't exists\n");
        exit(EXIT_FAILURE);
    }

    int i;
    int j;

    double complex c;

    double complex z_tmp;

    char buffer[50];

    for(i=0;i<N;i++){
        c = rand_re() + rand_im()*I;
        z_tmp = 0;

        j=0;
        while( cabs(z_tmp) <= 2 && j<K ){
            z_tmp = z_tmp*z_tmp + c;
            j++;
        }

        sprintf(buffer,"%lf %lf %d",__real__ c, __imag__ c, j);
        write(pipe,buffer,51);
    }

    close(pipe);

    return 0;
}