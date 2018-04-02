#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int free_lock(int file, long int byte_num){
    struct flock lock;
    lock.l_len = 1;
    lock.l_whence=SEEK_SET;
    lock.l_start=byte_num;
    lock.l_type=F_UNLCK;
    return fcntl(file,F_SETLK,&lock);
}

void print_locks(int file){
    long int end = (long int)lseek(file,0,SEEK_END);
    long int i;

    struct flock lock;
    for(i=0;i<=end;i++){
        lock.l_type=F_RDLCK;
        lock.l_whence=SEEK_SET;
        lock.l_len=1;
        lock.l_start=i;
        fcntl(file,F_GETLK,&lock);
        if(lock.l_type!=F_UNLCK){
            printf("Byte: %ld has READ/WRITELOCK by %d \n", i, lock.l_pid);
        }
        else {
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_len = 1;
            lock.l_start = i;
            fcntl(file, F_GETLK, &lock);

            if (lock.l_type != F_UNLCK) {
                printf("Byte: %ld has READLOCK by %d \n", i, lock.l_pid);
            }
        }
    }
    printf("\n");
}

long int choose_byte_num(int file){
    long int byte_num;
    printf("Enter byte number: ");
    scanf("%ld",&byte_num);
    printf("\n");
    long int end = (long int)lseek(file,0,SEEK_END);
    lseek(file,0,SEEK_SET);
    if(byte_num<0 || byte_num>end)
        return -1;
    else return byte_num;
}

int set_read_lock(int file, int lock_type,long int byte_num){
    struct flock lock;
    lock.l_len=1;
    lock.l_whence = SEEK_SET;
    lock.l_type = F_RDLCK;
    lock.l_start = byte_num;
    if(fcntl(file,lock_type,&lock)==-1) {
        printf("Unable to lock this byte\n\n");
        return 1;
    }
    else return 0;
}

int set_write_lock(int file, int lock_type, long int byte_num){
    struct flock lock;
    lock.l_len=1;
    lock.l_pid=getpid();
    lock.l_whence = SEEK_SET;
    lock.l_type = F_WRLCK;
    lock.l_start = byte_num;
    if(fcntl(file,lock_type,&lock)==-1){
        printf("Unable to lock this byte\n\n");
        return 1;
    }
    else return 0;
}

void read_byte(int file,long int byte_num){
    if(set_read_lock(file,F_SETLK,byte_num)==1) return;

    unsigned char byte;
    lseek(file,byte_num,SEEK_SET);
    read(file,&byte,1);
    lseek(file,0,SEEK_SET);
    printf("%c\n",byte);

    free_lock(file,byte_num);

}

void write_byte(int file, long int byte_num, unsigned char byte){
    if(set_write_lock(file,F_SETLK,byte_num)==1) return;
    lseek(file,byte_num,SEEK_SET);
    write(file,&byte,1);
    lseek(file,0,SEEK_SET);

    free_lock(file,byte_num);
}

int menu(char *path){
    int file = open(path,O_RDWR);
    if(file<0){
        printf("Something wrong with your file \n");
        exit(1);
    }
    int option;
    int done;
    long int byte_num;
    unsigned char byte;

    while(1){
        printf("1. Set lock on read\n");
        printf("2. Set lock on write\n");
        printf("3. Show list with locked bytes\n");
        printf("4. Free lock\n");
        printf("5. Read byte\n");
        printf("6. Change byte\n");
        printf("0. Exit program\n\n");

        printf("Choose option: ");

        scanf("%d",&option);

        switch (option) {
            case 1:
                done=0;
                while(done==0) {
                    printf("\n 1. Forcing lock and exiting when unavailable\n");
                    printf(" 2. Waiting until lock will be available\n");
                    printf(" 0. Go to main menu\n\n");
                    printf(" Choose option: ");
                    scanf("%d", &option);
                    switch (option) {
                        case 1:
                            while (1){
                                byte_num = choose_byte_num(file);
                                if(byte_num>=0) break;
                                else printf("Entered wrong byte number!\n\n");
                            }
                            if(set_read_lock(file,F_SETLK,byte_num)==0)
                                printf("Locked to read\n\n");
                            done = 1;
                            break;

                        case 2:
                            while (1){
                                byte_num = choose_byte_num(file);
                                if(byte_num>=0) break;
                                else printf("Entered wrong byte number!\n\n");
                            }
                            if(set_read_lock(file,F_SETLKW,byte_num)==0)
                                printf("Locked to read\n\n");
                            done = 1;
                            break;

                        case 0:
                            done = 1;
                            break;

                        default:
                            printf("Something wrong with your selected option. Please try again\n\n");
                    }
                }
                break;

            case 2:
                done=0;
                while(done==0) {
                    printf("\n 1. Forcing lock and exiting when unavailable\n");
                    printf(" 2. Waiting until lock will be available\n");
                    printf(" 0. Go to main menu\n\n");
                    printf(" Choose option: ");
                    scanf("%d", &option);
                    switch (option) {
                        case 1:
                            while (1){
                                byte_num = choose_byte_num(file);
                                if(byte_num>=0) break;
                                else printf("Entered wrong byte number!\n\n");
                            }
                            if(set_write_lock(file,F_SETLK,byte_num)==0)
                                printf("Locked to write\n\n");
                            done = 1;
                            break;

                        case 2:
                            while (1){
                                byte_num = choose_byte_num(file);
                                if(byte_num>=0) break;
                                else printf("Entered wrong byte number!\n\n");
                            }
                            if(set_write_lock(file,F_SETLKW,byte_num)==0)
                                printf("Locked to write\n\n");
                            done = 1;
                            break;

                        case 0:
                            done = 1;
                            break;

                        default:
                            printf("Something wrong with your selected option. Please try again\n\n");
                    }
                }
                break;

            case 3:
                print_locks(file);
                break;

            case 4:
                while(1) {
                    byte_num = choose_byte_num(file);
                    if(byte_num>=0) break;
                    else printf("Entered wrong byte number!\n\n");
                }
                if(free_lock(file,byte_num)<0){
                    printf("You can't unlock this byte\n");
                }
                else printf("Unlock successfully\n");
                break;

            case 5:
                while(1) {
                    byte_num = choose_byte_num(file);
                    if(byte_num>=0) break;
                    else printf("Entered wrong byte number!\n\n");
                }
                read_byte(file,byte_num);
                break;

            case 6:
                while(1) {
                    byte_num = choose_byte_num(file);
                    if(byte_num>=0) break;
                    else printf("Entered wrong byte number!\n\n");
                }
                printf("Enter a char which you want to write: ");
                scanf(" %c",&byte);
                printf("\n");
                write_byte(file,byte_num,byte);
                break;

            case 0:
                close(file);
                return 0;
            default:
                printf("Something wrong with your selected option. Please try again\n\n");
        }
    }
}

int main(int argc, char**argv) {
    if(argc!=2){
        printf("Wrong number of parameters!\n");
        exit(1);
    }
    menu(argv[1]);

    return 0;
}