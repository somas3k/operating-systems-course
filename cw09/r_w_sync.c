#include "r_w_sync.h"

fifo *threads_fifo;

int *permissions;

int monitor_flag = 0;

int readers = 0;

int isWriting = 0;

int isRunning = 1;

pthread_mutex_t fifo_mutex, monitor_mutex, permission_mutex, end_work_mutex;
pthread_cond_t monitor_cond, permission_cond, end_work_cond;

int main(int argc, char **argv) {
    srand((unsigned int) time(NULL));
    args *args1 = parse_args(argc, argv);
    atexit(exit_handler);

    struct sigaction sa;
    sigemptyset(&(sa.sa_mask));
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf("Readers: %d, writers %d, info %d\n",args1->readers,args1->writers,info);

    generate_data();

    init_sync_helpers();

    threads_fifo = fifo_init();

    int r=args1->readers;
    int w=args1->writers;

    pthread_t *thread_ids = malloc((r+w)*sizeof(pthread_t));
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    permissions = malloc((r+w)*sizeof(int));

    reader_arg ** r_arg = malloc(r*sizeof(reader_arg*));
    int *w_arg = malloc(w*sizeof(int));

    for(int i = 0; i < ((args1->readers >= args1->writers) ? args1->readers : args1->writers); ++i){
        if(r){
            r_arg[i] = malloc(sizeof(reader_arg));
            r_arg[i]->divider = rand()% 5 + 2 ;
            r_arg[i]->id = i;
            pthread_create(&thread_ids[i], &attr, reader_thread_fun, r_arg[i]);
            r--;
        }
        if(w){
            w_arg[i] = i+args1->readers;
            pthread_create(&thread_ids[w_arg[i]], &attr, writer_thread_fun, &w_arg[i]);
            w--;
        }
    }

    free(r_arg);

    while(isRunning){
        pthread_mutex_lock(&monitor_mutex);
        while(!monitor_flag){
            pthread_cond_wait(&monitor_cond,&monitor_mutex);
        }
        monitor_flag--;
        pthread_mutex_unlock(&monitor_mutex);

        pthread_mutex_lock(&fifo_mutex);
        thread_info info = fifo_pop(threads_fifo);
        pthread_mutex_unlock(&fifo_mutex);

        pthread_mutex_lock(&end_work_mutex);
        while(isWriting)
            pthread_cond_wait(&end_work_cond, &end_work_mutex);

        if(info.function == READER) readers++;
        isWriting = 1;

        pthread_mutex_unlock(&end_work_mutex);

        pthread_mutex_lock(&permission_mutex);
        permissions[info.id] = 1;
        pthread_cond_broadcast(&permission_cond);
        pthread_mutex_unlock(&permission_mutex);
    }

    for (int i =0; i<(args1->readers+args1->writers); ++i){
        pthread_cancel(thread_ids[i]);
    }

    free(w_arg);
    free(thread_ids);
    free(permissions);

    exit(0);
}

void init_sync_helpers(){
    fifo_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    monitor_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    permission_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    end_work_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    monitor_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    permission_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    end_work_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
}
void sigint_handler(int signo){
   isRunning = 0;
}

void exit_handler(){
    free_fifo(threads_fifo);
}

void wait_for_perm(thread_info info){
    pthread_mutex_lock(&fifo_mutex);
    fifo_push(threads_fifo,info);
    pthread_mutex_unlock(&fifo_mutex);

    pthread_mutex_lock(&permission_mutex);

    pthread_mutex_lock(&monitor_mutex);
    monitor_flag++;
    pthread_cond_signal(&monitor_cond);
    pthread_mutex_unlock(&monitor_mutex);

    while( permissions[info.id] == 0){
        pthread_cond_wait(&permission_cond,&permission_mutex);
    }

    pthread_mutex_unlock(&permission_mutex);

}

void signal_end(thread_info info){
    pthread_mutex_lock(&end_work_mutex);

    switch (info.function){
        case WRITER :
            isWriting = 0;
            break;
        case READER:
            readers--;
            if(readers ==0 ) isWriting = 0;
            break;
        default:
            break;
    }
    permissions[info.id] = 0;
    pthread_cond_signal(&end_work_cond);
    pthread_mutex_unlock(&end_work_mutex);

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
    thread_info info;
    info.id = *(int*) arg;
    info.function = WRITER;
    //free(arg);
    while(1){
        wait_for_perm(info);
        writing();
        signal_end(info);
        sleep(1);
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
        printf("Reader id: %zu finished. Was looking for divisible by %d. (Index : Number) : \n%s Total: %d\n",pthread_self(),div,num_indexes, count);

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

    printf("Reader %zu finished. Was looking for divisible by %d. Found numbers: %d\n",pthread_self(),div,count);
    fflush(stdin);

}

void *reader_thread_fun(void *arg){
    reader_arg *r_arg = (reader_arg*) arg;
    int divider = r_arg->divider;
    thread_info info;
    info.id = r_arg->id;
    info.function = READER;
    free(arg);
    while(1){
        wait_for_perm(info);
        reading(divider);
        signal_end(info);
        sleep(1);
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
