#include "r_w_starving_writers.h"

sem_t write_sem;

sem_t counter_sem;

int readers;

int data[TAB_SIZE];

int info = 0;

void sigint_handler(int signo){
    int res = sem_destroy(&write_sem);
    res+=sem_destroy(&counter_sem);
    if(res < 0) {
        fprintf(stderr,"Error while destroying semaphores");
    }
}

int main(int argc, char **argv) {
    srand((unsigned int) time(NULL));
    args *args1 = parse_args(argc, argv);

    struct sigaction sa;
    sigemptyset(&(sa.sa_mask));
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler;
    sigaction(SIGABRT, &sa, NULL);

    printf("Readers: %d, writers %d, info %d\n",args1->readers,args1->writers,info);

    generate_data();

    sem_init(&write_sem, 0, 1);
    sem_init(&counter_sem, 0, 1);
    readers = 0;

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    int r=args1->readers;
    int w=args1->writers;

    reader_arg ** r_arg = malloc(r*sizeof(reader_arg*));

    for(int i = 0; i < ((args1->readers >= args1->writers) ? args1->readers : args1->writers); ++i){
        if(r){
            r_arg[i] = malloc(sizeof(reader_arg));
            r_arg[i]->divider = rand()% 5 + 2 ;
            pthread_create(&tid, &attr, reader_thread_fun, r_arg[i]);
            r--;
        }
        if(w){
            pthread_create(&tid, &attr, writer_thread_fun, NULL);
            w--;
        }
    }

    free(r_arg);
    pthread_exit(0);
}

void writing_info(){
    int N = rand()%TAB_SIZE+1;
    char *num_indexes = calloc((size_t)N*30,sizeof(char));
    char num_index[30];
    for (int n = 0; n < N; n++){
        int index = rand()%TAB_SIZE;
        int value = rand();
        data[index] = value;
        sprintf(num_index,"%d : %d\n",index, value);
        strcat(num_indexes,num_index);
    }

    printf("Writer id %zu finished. Changed (Index : Value):\n%s",pthread_self(),num_indexes);
    free(num_indexes);
    fflush(stdin);
}

void writing(){
    if(info){
        writing_info();
        return;
    }
    int N = rand()%TAB_SIZE + 1;
    for(int n = 0; n < N; ++n){
        data[rand()%TAB_SIZE] = rand();
    }
    printf("Writer id: %zu finished.\n",pthread_self());
}

void *writer_thread_fun(void *arg){
    while(1){
        sem_wait(&write_sem);
        writing();
        sem_post(&write_sem);
        //sleep(1);
    }
}

void reading_info_v2(int div){
    char *num_indexes = calloc(50*TAB_SIZE,sizeof(char));
    char *num_indexes_ = NULL;
    int count = 0;
    char num_index[50];
    for(int i =0; i < TAB_SIZE; ++i){
        if(data[i] % div == 0) {
            count++;
            size_t act_size = 0;
            for(int j =0; j<50;j++) num_index[j] = '\0';
            if(count != 1){ while(num_indexes[act_size]!='\0') act_size++; }
            int saved = sprintf(num_index,"%d : %d, \n",i,data[i]);
            num_indexes_ = strncat(num_indexes,num_index,(size_t)saved);
            if(!num_indexes_)
                perror("STRNCAT error");
        }
    }
    if(num_indexes!=NULL){
        printf("Reader id: %zu finished. (Index : Number) : \n%s Total: %d\n",pthread_self(),num_indexes, count);
        free(num_indexes);
    }
}

void reading(int div){
    if(info){
        reading_info_v2(div);
        return;
    }
    int count = 0;
    for (int i = 0; i < TAB_SIZE; i++){
        if(data[i] % div == 0){
            count++;
        }
    }

    printf("Reader %zu finished. Found numbers: %d\n",pthread_self(),count);
    fflush(stdin);

}



void *reader_thread_fun(void *arg){
    reader_arg *r_arg = (reader_arg*) arg;
    int divider = r_arg->divider;
    free(arg);
    while(1){
        sem_wait(&counter_sem);
        readers++;
        if( readers == 1 ) sem_wait(&write_sem);
        sem_post(&counter_sem);
        reading(divider);
        sem_wait(&counter_sem);
        readers--;
        if( readers == 0 ) sem_post(&write_sem);
        sem_post(&counter_sem);
        //sleep(1);
    }
}

args *parse_args(int argc, char **argv){
    if(argc > 6) return NULL;
    args *arguments = malloc(sizeof(args));
    arguments->readers = -1;
    arguments->writers = -1;
    for(int i = 1; i < argc; i++){
        if( (strcmp(argv[i],"-i") == 0) && info == 0 ) {
            info = 1;
            continue;
        }
        if( ((strcmp(argv[i],"-r") == 0) || (strcmp(argv[i],"-R") == 0)) && arguments->readers==-1){
            if(i != argc - 1) {
                if (sscanf(argv[i + 1], "%d", &(arguments->readers)) != 1) return NULL;
                i++;
            }
            continue;
        }
        if( ((strcmp(argv[i],"-w") == 0) || (strcmp(argv[i],"-W") == 0)) && arguments->writers==-1){
            if(i != argc - 1) {
                if (sscanf(argv[i + 1], "%d", &(arguments->writers)) != 1) return NULL;
                i++;
            }
            continue;
        }
    }
    if (arguments->readers == -1) arguments->readers = rand() % TAB_SIZE +1;
    if (arguments->writers == -1) arguments->writers = rand() % TAB_SIZE +1;

    return arguments;
}

void generate_data(){
    for(int i = 0; i < TAB_SIZE ; i++){
        data[i] = rand()%1000;
    }
}
