#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

char *get_abs_path(char *path){
    char* abs_path = malloc(PATH_MAX);
    realpath(path,abs_path);
    return abs_path;
}

void print_file_info(char *path,struct stat *fileStat){
    time_t t = fileStat->st_mtime;
    struct tm local_time;
    localtime_r(&t,&local_time);
    char time_to_print[80];
    strftime(time_to_print,sizeof(time_to_print),"%c",&local_time);
    printf("Path: %s \nSize: %ld \nFile Permissions: \t",path,(long int)(fileStat->st_size));
    printf( (fileStat->st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat->st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat->st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat->st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat->st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat->st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat->st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat->st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat->st_mode & S_IXOTH) ? "x" : "-");
    printf("\nLast Modification: %s\n\n",time_to_print);
}



int find(const char *path,int max_size){
    DIR *dir_stream = opendir(path);
    if(dir_stream == NULL){
        return -1;
    }
    char *curr_path=malloc(sizeof(char)*(strlen(path)+255));
    struct dirent* info = readdir(dir_stream);
    while(info!=NULL){
        if(strcmp(info->d_name,".") ==0 || strcmp(info->d_name,"..")==0) {
            info = readdir(dir_stream);
            continue;
        }
        strcpy(curr_path,path);
        if(curr_path[strlen(curr_path) - 1] != '/') strcat(curr_path, "/");
        strcat(curr_path,info->d_name);
        struct stat fileStat;
        int status = lstat(curr_path,&fileStat);

        if(status>=0 && S_ISDIR(fileStat.st_mode)==1){
            find(curr_path,max_size);
            info = readdir(dir_stream);
            continue;
        }

        if(status>=0 && S_ISREG(fileStat.st_mode)==1 && fileStat.st_size<=max_size){
            print_file_info(curr_path,&fileStat);
        }

        info = readdir(dir_stream);
    }
    closedir(dir_stream);
    free(curr_path);
    return 0;

}


int main(int argc, char **argv) {
    if(argc!=3){
        printf("Wrong number of parameters!!\n");
        exit(1);
    }

    char *abs_path=get_abs_path(argv[1]);

    struct stat fileStat;
    int status = stat(abs_path,&fileStat);
    if(status<0 || S_ISDIR(fileStat.st_mode) == 0){
        printf("Is not a directory!!\n");
        exit(1);
    }

    if(find(abs_path,atoi(argv[2]))!= 0){
        printf("Something wrong with reading folder tree structure!\n");
        exit(1);
    }

    free(abs_path);

    return 0;
}
