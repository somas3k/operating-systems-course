//
// Created by kwrobel on 04.06.17.
//

#ifndef LAB10SERVER_COMMON_H
#define LAB10SERVER_COMMON_H

#define UNIX_PATH_MAX 108
#define ASIZE sizeof(struct sockaddr)


typedef struct Client{
    struct sockaddr* addr;
    socklen_t aSize;
    char type;
    char* name;
    char sent;
    char rec;
}Client;

typedef struct Message{ // dla datagramow wysylam wszystko razem, zeby sie nie mieszaly
    char type;
    char name[21];
    char connectType;
    int ctn;
    int result;
}Message;

typedef enum messTypes{
    LOGIN = 0, RESULT = 1, FAILSIZE = 2,
    FAILNAME = 3, PING = 4, PONG = 5,
    REQ = 6, SUCCESS = 7, LOGOUT = 8
}messTypes;

typedef enum connectTypes{
    LOCAL = 0, WEB = 1
}connectTypes;

void print_error(const char* err){
    printf("Error! %s Errno: %d, %s\n", err, errno, strerror(errno));
    exit(3);
}

char* concat(const char* one, const char* two){
    char* new = malloc(sizeof(char) * (strlen(one) + strlen(two) + 1) );
    strcpy(new, one);
    strcat(new, two);
    return new;
}

#endif //LAB10SERVER_COMMON_H
