#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

typedef struct prog_info{
    char *progPath;
    char **args;
}prog_info;

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

void printCommand(prog_info *progInfo){
    int i=0;
    printf("%s ",progInfo->progPath);
    while(progInfo->args[i]!=NULL){
        printf("%s ",progInfo->args[i++]);
    }
    printf("\n");
}

prog_info *parseCommand(char *line, int length){
    while(line[0]==' '){
        line++;
        length--;
    }
    if(line[0]=='\n') return NULL;
    prog_info *progInfo = malloc(sizeof(struct prog_info));
    progInfo->progPath=getProgName(&line, length);
    char *args = strtok(line,"\n");
    progInfo->args=splitArgs(args);
    progInfo->args[0]=progInfo->progPath;

    return progInfo;
}

char *getCommand(char *line, char **saveptr, int *first){
    if(*first==1){
        (*first)=0;
        return strtok_r(line,"|\n",saveptr);
    }
    else return strtok_r(NULL,"|\n",saveptr);
}

void spawnProc(int in, int out, prog_info * progInfo){
    if(fork()==0){
        if(in!=0){
            dup2(in,0);
            close(in);
        }
        if(out!=1){
            dup2(out,1);
            close(out);
        }
        execvp(progInfo->progPath,progInfo->args);
    }
}

void clean(prog_info **progs, int N){
    for(int i=0;i<N;i++){
        free(progs[i]);
    }
    free(progs);
}

void interpreter(){
    ssize_t read;
    size_t length=0;
    char* line=NULL;
    //prog_info *progInfo;
    char *saveptr;
    printf("Enter command:\n");
    if((read = getline(&line,&length,stdin))!=-1){
        if(read>1){
            int first=1;
            char *command;

            prog_info **progs = NULL;
            int size = 0;
            while((command = getCommand(line,&saveptr,&first))!=NULL){
                size++;
                progs = realloc(progs,(size_t)size);
                progs[size-1]=parseCommand(command,(int)strlen(command));
            }
            if(size==1 && strcmp(progs[0]->progPath,"exit")==0) {

                exit(EXIT_SUCCESS);
            }
            int i;
            int fd[2];
            int in = 0;
            for(i = 0;i<size;i++){
                if(i==size-1){
                    spawnProc(in,1,progs[i]);
                }
                else {
                    pipe(fd);

                    spawnProc(in, fd[1], progs[i]);

                    close(fd[1]);

                    in = fd[0];
                }
            }

            wait(NULL);
            printf("\n");
            clean(progs,size);
        }
    }
}

int main(int argc, char **argv) {

    while(1){
        interpreter();
    }

    return 0;
}