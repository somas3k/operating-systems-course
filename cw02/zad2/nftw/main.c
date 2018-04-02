#define _XOPEN_SOURCE 500
#include <stdio.h>

#include <stdlib.h>
#include <sys/stat.h>

#include <time.h>
#include <ftw.h>
#include <limits.h>


static int max_size;

char *get_abs_path(char *path){
    char* abs_path = malloc(PATH_MAX);
    realpath(path,abs_path);
    return abs_path;
}

int print_file_info(const char *path,const struct stat *fileStat,int flag, struct FTW *s){
    if(flag==FTW_F && fileStat->st_size<=max_size) {
        time_t t = fileStat->st_mtime;
        struct tm local_time;
        localtime_r(&t, &local_time);
        char time_to_print[80];
        strftime(time_to_print, sizeof(time_to_print), "%c", &local_time);
        printf("Path: %s \nSize: %ld \nFile Permissions: \t", path, (long int)fileStat->st_size);
        printf((fileStat->st_mode & S_IRUSR) ? "r" : "-");
        printf((fileStat->st_mode & S_IWUSR) ? "w" : "-");
        printf((fileStat->st_mode & S_IXUSR) ? "x" : "-");
        printf((fileStat->st_mode & S_IRGRP) ? "r" : "-");
        printf((fileStat->st_mode & S_IWGRP) ? "w" : "-");
        printf((fileStat->st_mode & S_IXGRP) ? "x" : "-");
        printf((fileStat->st_mode & S_IROTH) ? "r" : "-");
        printf((fileStat->st_mode & S_IWOTH) ? "w" : "-");
        printf((fileStat->st_mode & S_IXOTH) ? "x" : "-");
        printf("\nLast Modification: %s\n\n", time_to_print);
    }
    return 0;
}



int find(const char *path){
    int result = nftw(path,print_file_info,50,FTW_PHYS);
    if(result!=0) return -1;
    return 0;
}


int main(int argc, char **argv) {
    if(argc!=3){
        printf("Wrong number of parameters!!\n");
        exit(1);
    }
    max_size=atoi(argv[2]);
    char *abs_path=get_abs_path(argv[1]);

    struct stat fileStat;
    int status = stat(abs_path,&fileStat);
    if(status<0 || S_ISDIR(fileStat.st_mode) == 0){
        printf("Is not a directory!!\n");
        exit(1);
    }

    if(find(abs_path)!= 0){
        printf("Something wrong with reading folder tree structure!\n");
        exit(1);
    }

    return 0;
}
