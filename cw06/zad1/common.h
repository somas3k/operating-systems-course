#include <time.h>
#include <stdlib.h>

#define ECHO 1
#define UPPER 2
#define TIME 3
#define LOGOUT 4
#define QUIT 5
#define ID 6
#define LOGIN 8
#define ERROR 7
#define EXIT_SERVER 9
#define SERVER 's'
#define MAX_LENGTH_MSG 1000
#define MAX_CLIENTS 20

struct bufor{
	long type;
	char msg[MAX_LENGTH_MSG];
	int id;
    pid_t pid;
};

#define MSG_SIZE sizeof(struct bufor)-sizeof(long)



const char keys[30] = {'a','b','c','d','e','f','g','h','i','j','k',
			'l','m','n','o','p','q','r','5','t','u','v','w','x',
			'y','z','1','2','3','4'};

char getKey(){
	return keys[rand()%(MAX_CLIENTS+9)];
}
