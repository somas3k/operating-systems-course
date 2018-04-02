//
// Created by kwrobel on 23.05.17.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

sigset_t mask;
pthread_mutex_t mutex;
pthread_t main_thread_id;

void exit_program(int status, char* message) {
    if(status == EXIT_SUCCESS) {
        printf("%s\n",message);
    } else {
        perror(message);
    }
    exit(status);
}
void signal_handler(int signum) {
    pthread_mutex_lock(&mutex);
    pthread_t id = pthread_self();
    printf("Catched singal %d, my id: %zu", signum, id);
    if(id == main_thread_id) {
        printf(" I'm main thread\n");
    } else {
        printf(" I'm worker thread\n");
    }
    fflush(stdin);
    pthread_mutex_unlock(&mutex);
}

void init_signals() {
    struct sigaction sa;
    sigemptyset(&(sa.sa_mask));
    sa.sa_flags = 0;
    sa.sa_handler = signal_handler;
    if(sigaction(SIGUSR1,&sa, NULL) == -1) {
        exit_program(EXIT_FAILURE,"Couldn't initialize signal handlers for SIGUSR1");
    }
    struct sigaction sa2;
    sigemptyset(&(sa2.sa_mask));
    sa2.sa_flags = 0;
    sa2.sa_handler = signal_handler;
    if(sigaction(SIGTERM,&sa2,NULL) == -1) {
        exit_program(EXIT_FAILURE,"Couldn't initialize signal handlers for SIGTERM");
    }
}

struct record{
    int id;
    char text[1024-sizeof(int)];
};
struct thread_info{
    pthread_t threadID;
    int recordID;
};

int nthreads;
char *file_name;
int nrecords;
char *key;
int signal_num;
int fd;
struct thread_info *threadIDs;

int parseArgs(int argc, char **argv);

void *search_key_v1( void *arg);
void *search_key_v2( void *arg);
void *search_key_v3( void *arg);

int get_signal(int n);

char *get_signal_str(int n);

void atExit(){
    close(fd);
    free(threadIDs);
}

int main(int argc, char **argv) {
    if (parseArgs(argc, argv) != 0){
        fprintf(stderr, "Something wrong with your arguments");
        exit(1);
    }

    init_signals();
    main_thread_id = pthread_self();
    pthread_mutex_init(&mutex,NULL);

    atexit(atExit);
    threadIDs = malloc(nthreads * sizeof(struct thread_info));

    //printf("%d %s %d %s\n", nthreads, file_name, nrecords, key);

    fd = open(file_name, O_RDONLY);
    if(fd == -1) {
        fprintf(stderr, "Something wrong with your file");
        exit(1);
    }
    int IDs[nthreads];

    for( int i = 0; i < nthreads; ++i ){
        IDs[i] = i;
        pthread_create(&(threadIDs[i].threadID), NULL, search_key_v3, IDs + i);
    }

    printf("Sending %s to process...\n",get_signal_str(signal_num));
    kill(getpid(),get_signal(signal_num));
    printf("Sent!\n");
    int i = 0;
    void *ret;
    while(i < nthreads){
        pthread_join(threadIDs[i].threadID,&ret);
        printf("Watek %d zakonczony\n",(int)threadIDs[i++].threadID);
    }

    return 0;
}

int get_signal(int n) {
    switch(n) {
        case 1:
            return SIGUSR1;
        case 2:
            return SIGTERM;
        case 3:
            return SIGKILL;
        default:
            return SIGSTOP;
    }
}
char *get_signal_str(int n) {
    switch(n) {
        case 1:
            return "SIGUSR1";
        case 2:
            return "SIGTERM";
        case 3:
            return "SIGKILL";
        default:
            return "SIGSTOP";
    }
}


int parseArgs(int argc, char **argv){
    if(argc != 6) return 1;
    if (sscanf (argv[1], "%d", &nthreads) != 1)
        return 1;
    file_name = argv[2];
    if (sscanf (argv[3], "%d", &nrecords) != 1)
        return 1;
    key = argv[4];
    if (sscanf (argv[5], "%d", &signal_num) != 1)
        return 1;
    return 0;
}

int search_record(struct record record1){
    //printf("Searching in record id: %d text: %s\n",record1.id,record1.text);
    char *saveptr, *word;
    char *ptr = record1.text;
    while(1){
        word = strtok_r(ptr," \0",&saveptr);
        if(word == NULL) break;
        if(ptr != NULL) ptr = NULL;
        //sleep(1);
        //printf("Checking word: %s \n",word);
        if(strcmp(word,key) == 0) return 0;
    }
    return 1;
}
void *search_key_v1( void *arg){
    pthread_setcanceltype(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

    int ID = *(int *) arg;
    struct record records[nrecords];
    int isAvailable = 1;
    int isFound = 0;
    int offset;
    int i = 0;
    threadIDs[ID].recordID = -1;
    while(isAvailable && isFound!=1){
        //printf("ID: %d isFound: %d\n", ID, isFound);
        offset = nrecords * ( ID + i++ * nthreads );
        //printf("ID: %d Offset %d\n", ID, offset);
        int j = 0;
        for(j; j<nrecords; ++j){
            if(pread(fd, &records[j], sizeof(struct record), offset*1024 + j*1024)==0){
                //printf("ERROR ID: %d\n", ID);
                isAvailable=0;
                break;
            }
            //printf("ID : %d, %d %s \n", ID, records[j].id,records[j].text);
        }
        if(isAvailable==0) j--;
        for(int k = 0; k<j; k++){
            if(search_record(records[k])==0){
                threadIDs[ID].recordID = records[k].id;
                isFound = 1;
                for(int l = 0; l< nthreads; l++){
                    if(threadIDs[l].threadID!=pthread_self()){
                        if(pthread_cancel(threadIDs[l].threadID)!=0)
                            perror("Error during canceling thread");
                    }
                }
                break;
            }
        }
        //sleep(5);

    }
    //printf("2 isFound: %d\n", isFound);
    if (isFound == 1) printf("Thread %d ID: %d found a key in record %d\n",(int)pthread_self(), ID, threadIDs[ID].recordID);
    //while(1) {}
    return &threadIDs[ID].recordID;
}

void *search_key_v2( void *arg){
    pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    //printf("1 TID: %d Iteration: %d \n", (int) pthread_self(), *(int *) arg);

    int ID = *(int *) arg;
    struct record records[nrecords];
    int isAvailable = 1;
    int isFound = 0;
    int offset;
    int i = 0;
    threadIDs[ID].recordID = -1;
    while(isAvailable && isFound!=1){
        //printf("1 isFound: %d\n", isFound);
        offset = nrecords * ( ID + i++ * nthreads );
        //printf("Offset %d\n", offset);
        int j;
        for(j = 0; j<nrecords; ++j){
            if(pread(fd, &records[j], sizeof(struct record), offset*1024+j*1024)==0){
                isAvailable=0;
                break;
            }
            //printf("%d %s \n", records[j].id,records[j].text);
        }
        if(isAvailable==0) j--;
        for(int k = 0; k<j; k++){
            if(search_record(records[k])==0){
                threadIDs[ID].recordID = records[k].id;
                //printf("Thread %d found a key in record %d\n",pthread_self(),rec_id);
                isFound = 1;
                for(int l = 0; l< nthreads; l++){
                    if(threadIDs[l].threadID!=pthread_self()){
                        if(pthread_cancel(threadIDs[l].threadID)!=0)
                            perror("Error during canceling thread");
                    }
                }
                break;
            }
        }
        pthread_setcanceltype(PTHREAD_CANCEL_ENABLE,NULL);
        pthread_testcancel();
        pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);
        sleep(1);

    }
    //printf("2 isFound: %d\n", isFound);
    if (isFound == 1) printf("Thread %d found a key in record %d\n",(int)pthread_self(),threadIDs[ID].recordID);
    //while(1) {}
    return &threadIDs[ID].recordID;
}

void *search_key_v3( void *arg){
    //printf("1 TID: %d Iteration: %d \n", (int) pthread_self(), *(int *) arg);

    int ID = *(int *) arg;
    struct record records[nrecords];
    int isAvailable = 1;
    int isFound = 0;
    int offset;
    int i = 0;
    threadIDs[ID].recordID = -1;
    while(isAvailable && isFound!=1){
        //printf("1 isFound: %d\n", isFound);
        offset = nrecords * ( ID + i++ * nthreads );
        //printf("Offset %d\n", offset);
        int j;
        for(j = 0; j<nrecords; ++j){
            if(pread(fd, &records[j], sizeof(struct record), offset*1024+j*1024)==0){
                isAvailable=0;
                break;
            }
            //printf("%d %s \n", records[j].id,records[j].text);
        }
        if(isAvailable==0) j--;
        for(int k = 0; k<j; k++){
            if(search_record(records[k])==0){
                threadIDs[ID].recordID = records[k].id;
                //printf("Thread %d found a key in record %d\n",pthread_self(),rec_id);
                isFound = 1;
                break;
            }
        }
        sleep(1);

    }
    //printf("2 isFound: %d\n", isFound);
    if (isFound == 1) printf("Thread %d found a key in record %d\n",(int)pthread_self(),threadIDs[ID].recordID);
    //while(1) {}
    return &threadIDs[ID].recordID;
}

