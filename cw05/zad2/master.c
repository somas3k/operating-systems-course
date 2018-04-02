//
// Created by kwrobel on 10.04.17.
//


#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <tgmath.h>


int R;

int **area;

int convert_re(double re){
    return abs(((R-1)*(-2-re)/3));
}

int convert_im(double im){
    return abs(((R-1)*(1-im)/2));
}

void fillArea(double re, double im, int iters){
    area[convert_re(re)][convert_im(im)]=iters;}

void printToFile(){
    FILE *data = fopen("data","w");
    int i,j;
    for(i=0;i<R;i++){
        for(j=0;j<R;j++){
            fprintf(data,"%d %d %d\n",i,j,area[i][j]);

        }
    }

    fclose(data);
}

void allocate_area(int R){
    area = malloc(R*sizeof(int*));
    int i = 0;
    for(i=0;i<R;i++){
        area[i]=calloc(R,sizeof(int));
    }
}

void clean(){
    for(int i = 0;i < R; i++){
        free(area[i]);
    }
    free(area);
}

int main(int argc, char **argv){
    if(argc!=3) exit(EXIT_FAILURE);
    char *pipe_path = argv[1];
    R = atoi(argv[2]);
    allocate_area(R);

    if(mkfifo(pipe_path,0666)!=0) exit(EXIT_FAILURE);
    int pipe = open(pipe_path,O_RDONLY);

    double re,im;
    int iters;

    char buffer[51];
    while(read(pipe,buffer,51)!=0){
        sscanf(buffer,"%lf %lf %d",&re,&im,&iters);
        fillArea(re,im,iters);
    }

    close(pipe);
    unlink(pipe_path);

    printToFile();

    clean();

    FILE *gnu = popen("gnuplot","w");
    fprintf(gnu,"set view map\n");
    fprintf(gnu,"set xrange [0:%d]\n",R-1);
    fprintf(gnu,"set yrange [0:%d]\n",R-1);
    fprintf(gnu,"plot 'data' with image\n");
    fflush(gnu);
    getc(stdin);

    pclose(gnu);
}

