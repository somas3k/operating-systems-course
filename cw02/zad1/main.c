#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>


static int rand_int(int n) {
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;

    do {
        rnd = rand();
    }
    while (rnd >= limit);
    return rnd % n;
}


void generate(char *path,int record_count,int record_size){
    FILE *file= fopen(path,"w");
    if(!file){
        printf("Zła ścieżka do pliku!!\n");
        exit(1);
    }
    unsigned char *tab = malloc(record_size*sizeof(unsigned char));
    FILE *random = fopen("/dev/random","r");
    if(!random){
        printf("Problem z /dev/random\n");
        exit(1);
    }
    int i;
    for(i=0;i<record_count;i++){
        fread(tab,sizeof(unsigned char),record_size,random);
        fwrite(tab,sizeof(unsigned char),record_size,file);
    }
    free(tab);
    fclose(random);
    fclose(file);
}

void sort_lib(char *path,int record_count,int record_size){
    FILE *file_r = fopen(path,"rw+");
    if(!file_r){
        printf("Zła ścieżka do pliku!!\n");
        exit(1);
    }
    unsigned char *r1 = malloc(record_size*sizeof(unsigned char));
    unsigned char *r2 = malloc(record_size*sizeof(unsigned char));
    int i,j;

    for(i=0;i<record_count-1;i++){
        for(j=0;j<record_count-i-1;j++){
            fseek(file_r,j*record_size,SEEK_SET);
            fread(r1,sizeof(unsigned char),record_size,file_r);
            fseek(file_r,(j+1)*record_size,SEEK_SET);
            fread(r2,sizeof(unsigned char),record_size,file_r);
            if(r1[0]>r2[0]){
                fseek(file_r,j*record_size,SEEK_SET);
                fwrite(r2,sizeof(unsigned char),record_size,file_r);
                fseek(file_r,(j+1)*record_size,SEEK_SET);
                fwrite(r1,sizeof(unsigned char),record_size,file_r);
            }
        }
    }
    free(r1);
    free(r2);
    fclose(file_r);
}

void shuffle_lib(char* path,int record_count,int record_size){
    srand(time(0));
    int i,j;
    FILE* file = fopen(path,"rw+");
    unsigned char *r1 = malloc(record_size*sizeof(unsigned char));
    unsigned char *r2 = malloc(record_size*sizeof(unsigned char));
    for(i=record_count-1;i>0;i--){
        j=rand_int(i+1);
        fseek(file,i*record_size,SEEK_SET);
        fread(r1,sizeof(unsigned char),record_size,file);
        fseek(file,j*record_size,SEEK_SET);
        fread(r2,sizeof(unsigned char),record_size,file);
        fseek(file,j*record_size,SEEK_SET);
        fwrite(r1,sizeof(unsigned char),record_size,file);
        fseek(file,i*record_size,SEEK_SET);
        fwrite(r2,sizeof(unsigned char),record_size,file);
    }
    free(r1);
    free(r2);
    fclose(file);
}

void sort_sys(char *path,int record_count, int record_size){
    int file = open(path,O_RDWR);

    unsigned char *r1 = malloc(record_size*sizeof(unsigned char));
    unsigned char *r2 = malloc(record_size*sizeof(unsigned char));
    int i,j;

    for(i=0;i<record_count-1;i++){
        for(j=0;j<record_count-i-1;j++){
            lseek(file,j*record_size,SEEK_SET);
            read(file,r1,record_size);
            lseek(file,(j+1)*record_size,SEEK_SET);
            read(file,r2,record_size);
            if(r1[0]>r2[0]){
                lseek(file,j*record_size,SEEK_SET);
                write(file,r2,record_size);
                lseek(file,(j+1)*record_size,SEEK_SET);
                write(file,r1,record_size);
            }
        }
    }
    free(r1);
    free(r2);
    close(file);
}

void shuffle_sys(char *path, int record_count, int record_size){
    int file = open(path,O_RDWR);
    srand(time(0));
    int i,j;
    unsigned char *r1 = malloc(record_size*sizeof(unsigned char));
    unsigned char *r2 = malloc(record_size*sizeof(unsigned char));
    for(i=record_count-1;i>0;i--){
        j=rand_int(i+1);
        lseek(file,i*record_size,SEEK_SET);
        read(file,r1,record_size);
        lseek(file,j*record_size,SEEK_SET);
        read(file,r2,record_size);
        lseek(file,j*record_size,SEEK_SET);
        write(file,r1,record_size);
        lseek(file,i*record_size,SEEK_SET);
        write(file,r2,record_size);
    }
    free(r1);
    free(r2);
    close(file);

}

int main(int argc, char** argv) {
    if(argc<5&&argc>6){
        printf("Złe argumenty wywołania!");
        exit(1);
    }
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    double u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    double s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    switch(argv[1][0]){
        case 's':
            getrusage(RUSAGE_SELF, &rusage);

            switch(argv[2][1]){
                case 'h':
                    printf("SHUFFLING WITH SYSTEM FUNCTIONS: \n COUNT OF RECORDS: %d  SIZE OF RECORD: %d\n",atoi(argv[4]),atoi(argv[5]));
                    shuffle_sys(argv[3],atoi(argv[4]),atoi(argv[5]));
                    break;
                case 'o':
                    printf("SORTING WITH SYSTEM FUNCTIONS: \n COUNT OF RECORDS: %d  SIZE OF RECORD: %d\n",atoi(argv[4]),atoi(argv[5]));
                    sort_sys(argv[3],atoi(argv[4]),atoi(argv[5]));
                    break;
                default:
                    printf("Złe argumenty wywołania");
                    break;
            }
            break;
        case 'l':
            switch(argv[2][1]) {
                case 'h':
                    printf("SHUFFLING WITH FUNCTIONS FROM LIBRARY: \n COUNT OF RECORDS: %d  SIZE OF RECORD: %d\n",atoi(argv[4]),atoi(argv[5]));
                    shuffle_lib(argv[3],atoi(argv[4]),atoi(argv[5]));
                    break;

                case 'o':
                    printf("SORTING WITH FUNCTIONS FROM LIBRARY: \n COUNT OF RECORDS: %d  SIZE OF RECORD: %d\n",atoi(argv[4]),atoi(argv[5]));
                    sort_lib(argv[3],atoi(argv[4]),atoi(argv[5]));
                    break;

                default:
                    printf("Złe argumenty wywołania");
                    break;
            }
            break;
        case 'g':
            generate(argv[2],atoi(argv[3]),atoi(argv[4]));
            break;

    }

    getrusage(RUSAGE_SELF, &rusage);
    double u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    double s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    if(argv[1][0]!='g') {
        printf("user: %lf ", u_e - u_s);
        printf("sys: %lf\n\n", s_e - s_s);
    }
    return 0;
}
