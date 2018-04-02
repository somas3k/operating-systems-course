#define _XOPEN_SOURCE 1000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <limits.h>

extern char ** environ;

char *convertToEnv(char* arg){
    if(arg[0]=='$'){
        arg+=1;
        return getenv(arg);
    }
    else return arg;
}

char *searchInPath(char *prog_name){
    char *paths = strdup(getenv("PATH"));
    char *dir_path = strtok(paths,":");
    while(dir_path!=NULL){
        DIR *dir_stream = opendir(dir_path);
        if(dir_stream==NULL){
            dir_path=strtok(NULL,":");
            continue;
        }
        struct dirent *info = readdir(dir_stream);
        while(info!=NULL){
            if(strcmp(info->d_name,prog_name) ==0) return dir_path;
            info=readdir(dir_stream);
        }
        dir_path=strtok(NULL,":");
    }
    return NULL;
}

char **splitArgs(char *args) {
    int count_args = 1;
    char **argv=malloc(sizeof(char*));
    if (args != NULL) {
        while (args[0] == ' ') {
          args += 1;
        }
        char *tmp = strtok(args," ");
        while(tmp!=NULL){
            count_args++;
            argv=realloc(argv,(count_args)*sizeof(char*));
            argv[count_args-1]=strdup(tmp);
            tmp=strtok(NULL," ");
        }
    }
    else{
        argv[0]=NULL;
    }
    count_args++;
    argv=realloc(argv,count_args*sizeof(char*));
    argv[count_args-1]=NULL;
    return argv;
}

char *getProgName(char **line,int length){
    char *prog_name=NULL;
    int pos = 0;
    int size = 0;
    while(pos<length&&!((*line)[pos]!='\\'&&(((*line)[pos+1]==' ')||(*line)[pos+1]=='\n'))){
        if((*line)[size]=='\\'&&(((*line)[size+1]==' ')||((*line)[size+1]=='$')||((*line)[size+1]=='\\'))){
            prog_name=realloc(prog_name,(size+1)*sizeof(char));
            prog_name[size++]=(*line)[pos+1];
            pos=pos+2;
        }
        else{
            prog_name=realloc(prog_name,(size+1)*sizeof(char));
            prog_name[size++]=(*line)[pos++];
        }
    }
    if(pos+1==length){
        (*line)=NULL;
    }
    else {
        prog_name = realloc(prog_name, (size + 1) * sizeof(char));
        prog_name[size] = (*line)[pos++];
        (*line) += pos + 1;
    }
    return prog_name;

}

void check_for_env_var(char ** args){
    char **tmp = args;
    int i=1;
    while(tmp[i]!=NULL){
        if(convertToEnv(tmp[i])==NULL){
            printf("%s variable doesn't exist!\n",tmp[i]+1);
            exit(EXIT_FAILURE);
        }
        tmp[i]=convertToEnv(tmp[i]);
        i++;
    }

}

void parseLine(char *line,int length){
    while(line[0]==' '){
        line++;
        length--;
    }
    if(line[0]=='\n') return;
    if(line[0]=='#'){
        line+=1;
        char *var_name=strtok(line," \n");
        char *var_value=strtok(NULL,"\n");
        if(var_value==NULL) unsetenv(var_name);
        else {
            var_value=convertToEnv(var_value);
            setenv(var_name, var_value, 1);
        }
        return;
    }
    char *prog_name = getProgName(&line,length);
    prog_name = convertToEnv(prog_name);
    char *args = strtok(line,"\n");
    char *real_path;
    if(prog_name[0]=='/') real_path=prog_name;
    else {
        if ((prog_name[0] == '.' && prog_name[1] == '/') ||
            (prog_name[0] == '.' && prog_name[1] == '.' && prog_name[2] == '/')) {
            real_path = malloc(sizeof(char) * PATH_MAX);
            realpath(prog_name, real_path);
        } else {
            real_path = searchInPath(prog_name);
            if (real_path == NULL) {
                printf("%s didn't find program", prog_name);
                exit(EXIT_FAILURE);
            }
            strcat(real_path, "/");
            strcat(real_path, prog_name);

        }
    }
    char **argv = splitArgs(args);
    argv[0]=prog_name;
    check_for_env_var(argv);
    int pid = fork();
    if(pid==0){
        
        execve(real_path,argv,environ);
    }
    int status;

    wait(&status);

    printf("Program %s ends with status: %d\n\n",prog_name,status);
    if(status!=0) exit(EXIT_FAILURE);
    
}

void interpreter(char *path){
    FILE *file = fopen(path,"r");
    if(file==NULL){
        printf("Something wrong with your file\n");
        exit(EXIT_FAILURE);
    }
    ssize_t read;
    size_t length=0;
    char* line=NULL;
    while((read = getline(&line,&length,file))!=-1){
        if(read>1){
            parseLine(line,(int)read);
        }
    }
}

int main(int argc, char **argv) {
    if(argc!=2) {
        printf("Wrong parameters\n");
        exit(EXIT_FAILURE);
    }
    interpreter(argv[1]);
    return 0;
}
